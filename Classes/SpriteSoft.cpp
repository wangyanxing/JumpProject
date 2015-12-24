#include "SpriteSoft.h"
#include "LogicManager.h"

USING_NS_CC;

static const int NUM_SEGMENTS = 20;
static const float PHYSICS_BODY_RADIUS = 2;
static const float INNER_STIFFNESS = 1500;
static const float INNER_DAMPING = 50;
static const float OUTER_STIFFNESS = 1000;
static const float OUTER_DAMPING = 50;

SpriteSoft *SpriteSoft::create(const std::string &filename) {
  SpriteSoft * sprite = new(std::nothrow) SpriteSoft();
  if (sprite && sprite->initWithFile(filename.c_str())) {
    sprite->autorelease();
    return sprite;
  }
  CC_SAFE_DELETE(sprite);
  return nullptr;
}

bool SpriteSoft::initWithFile(const char *pszFilename) {
  if (Sprite::initWithFile(pszFilename)) {
    initPhysics();
    return true;
  }
  return false;
}


bool SpriteSoft::initWithSpriteFrameName(const char *pszFilename) {
  if (Sprite::initWithSpriteFrameName(pszFilename)) {
    initPhysics();
    return true;
  }
  return false;
}

void SpriteSoft::initPhysics(void) {
  setGLProgramState(
      GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));

  float bubbleRadius = getContentSize().width / 2;

  auto body = PhysicsBody::createCircle(PHYSICS_BODY_RADIUS);
  body->setRotationEnable(false);
  setPhysicsBody(body);

  auto phyWorld = GameLogic::PhysicsWorld;

  float childDist = bubbleRadius - PHYSICS_BODY_RADIUS;

  for (int i = 0; i < NUM_SEGMENTS; i++) {
    float childAngle = i * 2 * M_PI / NUM_SEGMENTS;

    Node *child = Node::create();
    child->setAnchorPoint(Vec2(0.5f, 0.5f));

    auto phybody = PhysicsBody::createCircle(PHYSICS_BODY_RADIUS);
    phybody->setRotationEnable(false);
    child->setPhysicsBody(phybody);
    child->setPosition(bubbleRadius + childDist * cosf(childAngle),
                       bubbleRadius + childDist * sinf(childAngle));
    addChild(child);

    auto j = PhysicsJointSpring::construct(body, phybody,
                                           Vec2::ZERO, Vec2::ZERO,
                                           INNER_STIFFNESS, INNER_DAMPING);
    j->setRestLength(childDist);
    phyWorld->addJoint(j);
  }

  for (int i = 0; i < NUM_SEGMENTS; i++) {
    Node *previous = i == 0 ? _children.at(NUM_SEGMENTS - 1) : _children.at(i - 1);
    Node *child = _children.at(i);
    auto j = PhysicsJointSpring::construct(child->getPhysicsBody(), previous->getPhysicsBody(),
                                           Vec2::ZERO, Vec2::ZERO, OUTER_STIFFNESS, OUTER_DAMPING);
    j->setRestLength(childDist * 2 * M_PI / NUM_SEGMENTS);
    phyWorld->addJoint(j);
  }
}

void SpriteSoft::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
  _customCommand.init(_globalZOrder);
  _customCommand.func = CC_CALLBACK_0(SpriteSoft::onDraw, this, transform, flags);
  renderer->addCommand(&_customCommand);
}

void SpriteSoft::onDraw(const cocos2d::Mat4 &transform, uint32_t flags) {
  const int nVertices = NUM_SEGMENTS + 2;

  float deltaAngle = (2.f * M_PI) / NUM_SEGMENTS;
  float bubbleRadius = getContentSize().width / 2;

  // Triangle fan vertices
  Vec2 vertices[nVertices];
  vertices[0].set(bubbleRadius, bubbleRadius);

  for (int i = 0; i < NUM_SEGMENTS; i++) {
    Node *child = _children.at(i);
    vertices[i + 1].set(
        child->getPositionX() +
        PHYSICS_BODY_RADIUS * (child->getPositionX() - bubbleRadius) / bubbleRadius,
        child->getPositionY() +
        PHYSICS_BODY_RADIUS * (child->getPositionY() - bubbleRadius) / bubbleRadius
    );
  }
  vertices[NUM_SEGMENTS + 1] = vertices[1];

  // Corresponding texture coordinates
  Vec2 texCoords[nVertices];
  texCoords[0].set(0.5f, 0.5f);
  for (int i = 0; i < NUM_SEGMENTS; i++) {
    GLfloat coordAngle = M_PI + (deltaAngle * i);
    texCoords[i + 1].set(0.5 + cosf(coordAngle) * 0.5, 0.5 + sinf(coordAngle) * 0.5);
  }
  texCoords[NUM_SEGMENTS + 1] = texCoords[1];

  // Default colors
  Color4F colors[nVertices];
  for (int i = 0; i <= NUM_SEGMENTS + 1; i++) {
    colors[i] = Color4F(getColor());
    colors[i].a = getOpacity() / 255.0f;
  }

  auto glProgram = getGLProgram();
  glProgram->use();
  getGLProgramState()->applyUniforms();
  glProgram->setUniformsForBuiltins(transform);

  GL::blendFunc(_blendFunc.src, _blendFunc.dst);
  GL::bindTexture2D(_texture->getName());
  GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_TRUE, 0, colors);
  glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) nVertices);

  CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, nVertices);
  CHECK_GL_ERROR_DEBUG();
}

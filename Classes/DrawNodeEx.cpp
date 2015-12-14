
#include "DrawNodeEx.h"
//#include "CCGL.h"
#include "base/CCEventType.h"
#include "base/CCConfiguration.h"
#include "renderer/CCCustomCommand.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "renderer/CCTextureCache.h"

NS_CC_BEGIN

// Vec2 == CGPoint in 32-bits, but not in 64-bits (OS X)
// that's why the "v2f" functions are needed
static Vec2 v2fzero(0.0f,0.0f);

static inline Vec2 v2f(float x, float y) {
  Vec2 ret(x, y);
  return ret;
}

static inline Vec2 v2fadd(const Vec2 &v0, const Vec2 &v1) {
  return v2f(v0.x+v1.x, v0.y+v1.y);
}

static inline Vec2 v2fsub(const Vec2 &v0, const Vec2 &v1) {
  return v2f(v0.x-v1.x, v0.y-v1.y);
}

static inline Vec2 v2fmult(const Vec2 &v, float s) {
  return v2f(v.x * s, v.y * s);
}

static inline Vec2 v2fperp(const Vec2 &p0) {
  return v2f(-p0.y, p0.x);
}

static inline Vec2 v2fneg(const Vec2 &p0) {
  return v2f(-p0.x, - p0.y);
}

static inline float v2fdot(const Vec2 &p0, const Vec2 &p1) {
  return  p0.x * p1.x + p0.y * p1.y;
}

static inline Vec2 v2fforangle(float _a_) {
  return v2f(cosf(_a_), sinf(_a_));
}

static inline Vec2 v2fnormalize(const Vec2 &p) {
  Vec2 r = Vec2(p.x, p.y).getNormalized();
  return v2f(r.x, r.y);
}

static inline Vec2 __v2f(const Vec2 &v) {
  //#ifdef __LP64__
  return v2f(v.x, v.y);
  // #else
  // 	return * ((Vec2*) &v);
  // #endif
}

static inline Tex2F __t(const Vec2 &v) {
  return *(Tex2F*)&v;
}

// implementation of DrawNodeEx

DrawNodeEx::DrawNodeEx()
: _vao(0)
, _vbo(0)
, _bufferCapacity(0)
, _bufferCount(0)
, _buffer(nullptr)
, _dirty(false) {
  _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
}

DrawNodeEx::~DrawNodeEx() {
  CC_SAFE_RELEASE(_texture);

  free(_buffer);
  _buffer = nullptr;

  glDeleteBuffers(1, &_vbo);
  _vbo = 0;

  if (Configuration::getInstance()->supportsShareableVAO()) {
    glDeleteVertexArrays(1, &_vao);
    GL::bindVAO(0);
    _vao = 0;
  }
}

DrawNodeEx* DrawNodeEx::create(const std::string& texture) {
  DrawNodeEx* ret = new DrawNodeEx();
  if (ret && ret->init(texture)) {
    ret->autorelease();
  } else {
    CC_SAFE_DELETE(ret);
  }

  return ret;
}

void DrawNodeEx::ensureCapacity(int count) {
  CCASSERT(count>=0, "capacity must be >= 0");
  if(_bufferCount + count > _bufferCapacity) {
    _bufferCapacity += MAX(_bufferCapacity, count);
    _buffer = (V2F_C4B_T2F*)realloc(_buffer, _bufferCapacity*sizeof(V2F_C4B_T2F));
  }
}

bool DrawNodeEx::init(const std::string& texture) {
  _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

  _texture = Director::getInstance()->getTextureCache()->addImage(texture);

  setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));

  ensureCapacity(512);

  if (Configuration::getInstance()->supportsShareableVAO()) {
    glGenVertexArrays(1, &_vao);
    GL::bindVAO(_vao);
  }

  glGenBuffers(1, &_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);

  glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, vertices));

  glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, colors));

  glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
  glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (Configuration::getInstance()->supportsShareableVAO()) {
    GL::bindVAO(0);
  }

  CHECK_GL_ERROR_DEBUG();

  _dirty = true;

#if CC_ENABLE_CACHE_TEXTURE_DATA
  // Need to listen the event only when not use batchnode, because it will use VBO
  auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [&](EventCustom* event){
    /** listen the event that renderer was recreated on Android/WP8 */
    this->init(texture);
  });

  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

  return true;
}

void DrawNodeEx::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
  _customCommand.init(_globalZOrder);
  _customCommand.func = CC_CALLBACK_0(DrawNodeEx::onDraw, this, transform, flags);
  renderer->addCommand(&_customCommand);
}

void DrawNodeEx::onDraw(const Mat4 &transform, uint32_t flags) {
  auto glProgram = getGLProgram();
  glProgram->use();
  getGLProgramState()->applyUniforms();
  glProgram->setUniformsForBuiltins(transform);

  GL::blendFunc(_blendFunc.src, _blendFunc.dst);
  GL::bindTexture2D( _texture->getName() );

  if (_dirty) {
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)*_bufferCapacity, _buffer, GL_STREAM_DRAW);
    _dirty = false;
  }
  if (Configuration::getInstance()->supportsShareableVAO()) {
    GL::bindVAO(_vao);
  } else {
    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    // vertex
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, vertices));

    // color
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, colors));

    // texcood
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F), (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));
  }

  glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,_bufferCount);
  CHECK_GL_ERROR_DEBUG();
}

void DrawNodeEx::drawTriangles(const std::vector<V2F_C4B_T2F_Triangle>& triangles) {
  unsigned vertex_count = (unsigned)triangles.size() * 3;
  ensureCapacity(vertex_count);

  CC_ASSERT(!triangles.empty());

  V2F_C4B_T2F_Triangle *t = (V2F_C4B_T2F_Triangle *)(_buffer + _bufferCount);
  memcpy((void*)t, &triangles[0], sizeof(V2F_C4B_T2F_Triangle) * triangles.size());

  _bufferCount += vertex_count;
  _dirty = true;
}

void DrawNodeEx::clear() {
  _bufferCount = 0;
  _dirty = true;
}

const BlendFunc& DrawNodeEx::getBlendFunc() const {
  return _blendFunc;
}

void DrawNodeEx::setBlendFunc(const BlendFunc &blendFunc) {
  _blendFunc = blendFunc;
}

NS_CC_END

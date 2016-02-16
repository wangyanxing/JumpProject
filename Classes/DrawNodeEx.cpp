
#include "DrawNodeEx.h"

NS_CC_BEGIN

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
  free(_buffer);
  _buffer = nullptr;

  glDeleteBuffers(1, &_vbo);
  _vbo = 0;

  if (Configuration::getInstance()->supportsShareableVAO()) {
    GL::bindVAO(0);
    glDeleteVertexArrays(1, &_vao);
    _vao = 0;
  }
}

DrawNodeEx* DrawNodeEx::create() {
  DrawNodeEx* ret = new (std::nothrow) DrawNodeEx();
  if (ret && ret->init()) {
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

bool DrawNodeEx::init() {
  _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

  setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(
      GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));

  ensureCapacity(1024);

  if (Configuration::getInstance()->supportsShareableVAO()) {
    glGenVertexArrays(1, &_vao);
    GL::bindVAO(_vao);
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
    // vertex
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_POSITION);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2,
                          GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, vertices));
    // color
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_COLOR);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, colors));
    // texcood
    glEnableVertexAttribArray(GLProgram::VERTEX_ATTRIB_TEX_COORD);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2,
                          GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));
    GL::bindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

  } else {
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V2F_C4B_T2F)* _bufferCapacity, _buffer, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  CHECK_GL_ERROR_DEBUG();

  _dirty = true;

#if CC_ENABLE_CACHE_TEXTURE_DATA
  // Need to listen the event only when not use batchnode, because it will use VBO
  auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
    /** listen the event that renderer was recreated on Android/WP8 */
    this->init(texture);
  });

  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif

  return true;
}

void DrawNodeEx::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
  if(_bufferCount) {
    _customCommand.init(_globalZOrder, transform, flags);
    _customCommand.func = CC_CALLBACK_0(DrawNodeEx::onDraw, this, transform, flags);
    renderer->addCommand(&_customCommand);
  }
}

void DrawNodeEx::onDraw(const Mat4 &transform, uint32_t flags) {
  auto glProgram = getGLProgram();
  glProgram->use();
  glProgram->setUniformsForBuiltins(transform);

  GL::blendFunc(_blendFunc.src, _blendFunc.dst);

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
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2,
                          GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, vertices));
    // color
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4,
                          GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, colors));
    // texcood
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2,
                          GL_FLOAT, GL_FALSE, sizeof(V2F_C4B_T2F),
                          (GLvoid *)offsetof(V2F_C4B_T2F, texCoords));
  }

  glDrawArrays(GL_TRIANGLES, 0, _bufferCount);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (Configuration::getInstance()->supportsShareableVAO()) {
    GL::bindVAO(0);
  }

  CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _bufferCount);
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

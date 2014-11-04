
#ifndef __CCDRAWNODES_CCDRAW_PLUS_NODE_H__
#define __CCDRAWNODES_CCDRAW_PLUS_NODE_H__

#include "2d/CCNode.h"
#include "base/ccTypes.h"
#include "renderer/CCCustomCommand.h"

NS_CC_BEGIN

class CC_DLL DrawNodeEx : public Node
{
public:
    /** creates and initialize a DrawNodeEx node */
    static DrawNodeEx* create(const std::string& texture);
    
    void drawTriangles(const std::vector<V2F_C4B_T2F_Triangle>& triangles);
    
    /** Clear the geometry in the node's buffer. */
    void clear();
    /**
     * @js NA
     * @lua NA
     */
    const BlendFunc& getBlendFunc() const;
    /**
     * @code
     * When this function bound into js or lua,the parameter will be changed
     * In js: var setBlendFunc(var src, var dst)
     * @endcode
     * @lua NA
     */
    void setBlendFunc(const BlendFunc &blendFunc);
    
    void onDraw(const Mat4 &transform, uint32_t flags);
    
    // Overrides
    virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;
    
    Texture2D* getTexture() { return _texture; }
    
CC_CONSTRUCTOR_ACCESS:
    DrawNodeEx();
    virtual ~DrawNodeEx();
    virtual bool init(const std::string& texture);
    
protected:
    void ensureCapacity(int count);
    
    GLuint      _vao;
    GLuint      _vbo;
    
    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V2F_C4B_T2F *_buffer;
    
    BlendFunc   _blendFunc;
    CustomCommand _customCommand;
    
    Texture2D* _texture{ nullptr };
    
    bool        _dirty;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawNodeEx);
};

NS_CC_END

#endif

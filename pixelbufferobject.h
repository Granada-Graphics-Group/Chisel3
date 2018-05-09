#ifndef PIXELBUFFEROBJECT_H
#define PIXELBUFFEROBJECT_H

#include "bufferobject.h"

#include <GL/glew.h>
#include <vector>

class PixelBufferObject :  BufferObject
{
public:
    PixelBufferObject(unsigned int capacity, bool mode = 0);
    ~PixelBufferObject();
    
    GLuint id() const { return mId; }
    GLuint capacity() const { return mCapacity; }
    std::vector<glm::byte>& dataCache(){ return mDataCache; }
    
    virtual void dispose();
    virtual void bind();
    virtual void unmap();
    virtual glm::byte* map(unsigned int requestedCapacity);
    
    void readPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type);
    
private:
    void setCapacity(int capacity);
    
    GLenum mTarget = GL_PIXEL_PACK_BUFFER;
    GLuint mId = 0;
    GLuint mCapacity = 0; 
    GLbitfield mMapFlags = GL_MAP_READ_BIT;
    GLbitfield mCreateFlags = GL_DYNAMIC_DRAW;
    glm::byte* mData;
    std::vector<glm::byte> mDataCache;
};

#endif // PIXELBUFFEROBJECT_H

#ifndef ARRAYBUFFEROBJECT_H
#define ARRAYBUFFEROBJECT_H

//#include "bufferobject.h"

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <vector>

class ArrayBufferObject
{
public:
    ArrayBufferObject(GLenum type, GLuint componentCount, GLuint divisor) : ArrayBufferObject(type, componentCount, divisor, 0, nullptr){};
    ArrayBufferObject(GLenum type, GLuint componentCount, GLuint divisor, GLsizei requestedCapacity, GLvoid* data = nullptr) : ArrayBufferObject(GL_ARRAY_BUFFER, type, componentCount, divisor, requestedCapacity, data){};
    ArrayBufferObject(GLenum target, GLenum type, GLuint componentCount, GLuint divisor, GLsizei requestedCapacity, GLvoid* data = nullptr);
    
    ~ArrayBufferObject();
    
    GLuint id() const { return mId; }
    GLenum type() const { return mType; }
    GLsizei componentSize() const { return mComponentSize; }
    GLuint componentCount() const { return mComponentCount; }
    GLuint divisor() const { return mDivisor; }
    GLsizei capacity() const { return mCapacity; }
    GLuint bindingIndex() const { return mBindingIndex; }
    
    virtual void dispose();
    virtual void bind();
    virtual void unmap();
    void resize(GLsizei requestedCapacity);
    glm::byte* map(GLsizei requestedCapacity);

    void update(GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void setFullCacheUpdate();
    void updateCache(GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void updateGPU();
    void bindToIndexedBufferTarget(GLuint index, GLintptr offset, GLsizeiptr size);


private:
    void setCapacity(GLsizei capacity);
    GLsizei sizeOf(GLenum type);

    GLuint mId = 0;    
    GLenum mTarget;
    GLenum mType; 
    GLsizei mComponentSize;
    GLuint mComponentCount;    
    GLuint mDivisor;
    GLsizei mCapacity = 0;
    GLuint mBindingIndex = 0;
    GLbitfield mMapFlags = GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT;
    GLbitfield mCreateFlags = GL_STREAM_DRAW;
    glm::byte* mData;
    
    std::vector<glm::byte> mDataCache;
    GLintptr mUpdateOffset = 0;
    GLsizeiptr mUpdateSize = 0;    
};

#endif // ARRAYBUFFEROBJECT_H

#ifndef UNIFORMBUFFEROBJECT_H
#define UNIFORMBUFFEROBJECT_H

#include "bufferobject.h"

#include <GL/glew.h>
#include <vector>


class UniformBufferObject : public BufferObject
{
public:
    UniformBufferObject(unsigned int requestedCapacity, GLvoid* data = nullptr) : UniformBufferObject(requestedCapacity, data, 1){};
    UniformBufferObject(unsigned int requestedCapacity, GLvoid* data, std::size_t componentSize);
    ~UniformBufferObject();
    
    virtual void dispose();
    virtual void bind();
    virtual void unmap();
    virtual glm::byte* map(unsigned int requestedCapacity);    
    void update(GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void updateCache(GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void updateGPU();
    void bindToIndexedBufferTarget(GLuint index, GLintptr offset, GLsizeiptr size);
    GLuint id() const { return mId; }
    GLuint capacity() const { return mCapacity; }
    GLuint bindingIndex() const { return mBindingIndex; }
    GLsizei componentSize() const { return mComponentSize; }
    
private:
    void setCapacity(int capacity);
    
    GLenum mTarget = GL_UNIFORM_BUFFER;
    GLuint mId = 0;
    GLuint mCapacity = 0;
    GLuint mBindingIndex = 0;
    GLbitfield mMapFlags = GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT;
    GLbitfield mCreateFlags = GL_STREAM_DRAW;
    glm::byte* mData;
    
    GLsizei mComponentSize;
    std::vector<glm::byte> mDataCache;
    GLintptr mUpdateOffset = 0;
    GLsizeiptr mUpdateSize = 0;
};

#endif // UNIFORMBUFFEROBJECT_H

#ifndef PERSISTENTBUFFEROBJECT_H
#define PERSISTENTBUFFEROBJECT_H

#include "bufferobject.h"

#include <GL/glew.h>


class PersistentBufferObject : public BufferObject
{
public:
    PersistentBufferObject(GLenum target, unsigned int requestedCapacity = 3);
    ~PersistentBufferObject();
    
    glm::byte* map(unsigned int requestedCapacity = 3) override;
    void unmap() override;
    void bind() override;
    void dispose() override;
    glm::byte* dataPointer(){ return mData; }
    GLuint id() { return mId; }
    GLuint capacity() { return mCapacity; }
private:
    GLenum mTarget = GL_ARRAY_BUFFER;
    GLuint mId = 0;
    GLuint mCapacity = 0;
    GLbitfield mMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    GLbitfield mCreateFlags = mMapFlags | GL_DYNAMIC_STORAGE_BIT;
    glm::byte* mData = nullptr;
};

#endif // PERSISTENTBUFFEROBJECT_H

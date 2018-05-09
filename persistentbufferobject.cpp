#include "persistentbufferobject.h"

// GL_MAP_DYNAMIC_STORAGE_BIT ?

PersistentBufferObject::PersistentBufferObject( GLenum target, unsigned int requestedCapacity )
:
    mTarget(target)
{
    map(requestedCapacity);
}


PersistentBufferObject::~PersistentBufferObject()
{
	glDeleteBuffers(1, &mId);
}

glm::byte* PersistentBufferObject::map( unsigned int requestedCapacity )
{
    mCapacity = requestedCapacity * 4;
    
    if (mData != nullptr)
    {
        glUnmapBuffer(mTarget);
        glDeleteBuffers(1, &mId);
    }
    
    glGenBuffers(1, &mId);
    glBindBuffer(mTarget, mId);
    glBufferStorage(mTarget, mCapacity, NULL, mCreateFlags);
    mData = static_cast<glm::byte *>(glMapBufferRange(mTarget, 0, mCapacity/4.0, mMapFlags));
    return mData;
}

void PersistentBufferObject::unmap()
{
    glDeleteBuffers(1, &mId);
}

void PersistentBufferObject::bind()
{
    glBindBuffer(mTarget, mId);
}

void PersistentBufferObject::dispose()
{
    glDeleteBuffers(1, &mId);
}

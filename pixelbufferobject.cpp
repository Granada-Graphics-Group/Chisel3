#include "pixelbufferobject.h"

PixelBufferObject::PixelBufferObject(unsigned int capacity, bool mode)
:
    mCapacity(capacity),
    mDataCache(capacity, 0)
{
    if(mode)
        mTarget = GL_PIXEL_UNPACK_BUFFER;
    
    glGenBuffers(1, &mId);
    glBindBuffer(mTarget, mId);
    glBufferData(mTarget, capacity, 0, mCreateFlags);
    glBindBuffer(mTarget, 0);
}

PixelBufferObject::~PixelBufferObject()
{
    glDeleteBuffers(1, &mId);
}

void PixelBufferObject::dispose()
{
    glDeleteBuffers(1, &mId);
}

void PixelBufferObject::bind()
{
    glBindBuffer(mTarget, mId);
}

void PixelBufferObject::unmap()
{
    glUnmapBuffer(mTarget);
}

glm::byte* PixelBufferObject::map(unsigned int requestedCapacity)
{
    glBindBuffer(mTarget, mId);
    if(mCapacity < requestedCapacity)
        setCapacity(requestedCapacity);

    mData = static_cast<glm::byte *>(glMapBufferRange(mTarget, 0, mCapacity, mMapFlags));

    memcpy(mDataCache.data(), mData, mCapacity);

    return mData;
}

// *** Private Methods *** //

void PixelBufferObject::setCapacity(int capacity)
{
    mCapacity = capacity;
    glBufferData(mTarget, mCapacity, NULL, mCreateFlags);
}
#include "unsynchronizedbufferobject.h"

UnsynchronizedBufferObject::UnsynchronizedBufferObject(GLenum target, unsigned int requestedCapacity)
    :
    mTarget(target),
    mMapFlags(GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT)
{
    glGenBuffers(1, &mId);
    glBindBuffer(mTarget, mId);
    glBufferData(mTarget, mCapacity, NULL, GL_STREAM_DRAW);
    mCapacity = 0;
}

UnsynchronizedBufferObject::~UnsynchronizedBufferObject()
{

}


void UnsynchronizedBufferObject::dispose()
{
    glDeleteBuffers(1, &mId);
}

void UnsynchronizedBufferObject::bind()
{
    glBindBuffer(mTarget, mId);
}

void UnsynchronizedBufferObject::unmap()
{
    glUnmapBuffer(mTarget);
}

void* UnsynchronizedBufferObject::map(int requestedCapacity)
{
    glBindBuffer(mTarget, mId);
    if(mCapacity < requestedCapacity)
        setCapacity(requestedCapacity);

    mData = glMapBufferRange(mTarget, 0, mCapacity, mMapFlags);

    return mData;
}

void UnsynchronizedBufferObject::setCapacity(int capacity)
{
    mCapacity = capacity;
    glBufferData(mTarget, mCapacity, NULL, GL_STREAM_DRAW);
}


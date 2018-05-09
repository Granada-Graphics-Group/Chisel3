#include "arraybufferobject.h"

ArrayBufferObject::ArrayBufferObject(GLenum target, GLenum type, GLuint componentCount, GLuint divisor, GLsizei requestedCapacity, GLvoid* data)
:
    mTarget(target),
    mType(type),
    mComponentSize(sizeOf(type)),
    mComponentCount(componentCount),
    mDivisor(divisor),
    mCapacity(requestedCapacity)        
{
    glCreateBuffers(1, &mId);
    
    if(mCapacity > 0)
    {
        mDataCache.resize(mCapacity);
        
        if(data != nullptr)
            std::copy(static_cast<const glm::byte *>(data), static_cast<const glm::byte *>(data) + mCapacity, begin(mDataCache));

        glNamedBufferData(mId, mCapacity, data, mCreateFlags);        
    }
    
    mUpdateOffset = mDataCache.size();
    mUpdateSize = 0;
}

ArrayBufferObject::~ArrayBufferObject()
{
    glDeleteBuffers(1, &mId);
}

void ArrayBufferObject::dispose()
{
    glDeleteBuffers(1, &mId);
}

void ArrayBufferObject::bind()
{
    glBindBuffer(mTarget, mId);
}

void ArrayBufferObject::unmap()
{
    glUnmapBuffer(mTarget);
}

void ArrayBufferObject::resize(GLsizei requestedCapacity)
{
    if(mCapacity < requestedCapacity)
        setCapacity(requestedCapacity);
}


glm::byte* ArrayBufferObject::map(GLsizei requestedCapacity)
{ 
    if(mCapacity < requestedCapacity)
        setCapacity(requestedCapacity);

    mData = static_cast<glm::byte *>(glMapNamedBufferRange(mId, 0, mCapacity, mMapFlags));

    return mData;
}

void ArrayBufferObject::update(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    glNamedBufferSubData(mId, offset, size, data);
}

void ArrayBufferObject::setFullCacheUpdate()
{
    mUpdateOffset  = 0;
    mUpdateSize = mCapacity;
}


void ArrayBufferObject::updateCache(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
    std::copy(byteData, byteData + size, begin(mDataCache) + offset);
    
    if (offset < mUpdateOffset)        
    {
        if(mUpdateSize != 0)
        {
            auto difference = (offset + size) - (mUpdateOffset + mUpdateSize);
            mUpdateSize = (difference >= 0) ? size : mUpdateSize + (mUpdateOffset - offset);
        }
        else
            mUpdateSize = size;
        
        mUpdateOffset = offset;
                    
/*        if(mUpdateSize != 0)
        {
            auto distance = mUpdateOffset - offset;
            
            if((size - distance) > mUpdateSize)
                mUpdateSize = size;
            else
                mUpdateSize += distance;
        }
        else
            mUpdateSize = size; */     
    }
    else if (offset > mUpdateOffset)
    {
        auto difference = (offset + size) - (mUpdateOffset + mUpdateSize);
        
        if (difference > 0) mUpdateSize += difference;
        
        //auto distance = offset - mUpdateOffset;
        //if((mUpdateSize - distance) < size)
        ////    mUpdateSize += distance;
        ////else
        //    mUpdateSize = distance + size;
    }
    else if (offset == mUpdateOffset && size > mUpdateSize)
        mUpdateSize = size;
}

void ArrayBufferObject::updateGPU()
{
    if(mUpdateSize > 0)
    {
        glNamedBufferSubData(mId, mUpdateOffset * sizeof(glm::byte), mUpdateSize * sizeof(glm::byte), mDataCache.data() + mUpdateOffset);
        
        mUpdateSize = 0;
        mUpdateOffset = mDataCache.capacity();
    }
}


void ArrayBufferObject::bindToIndexedBufferTarget(GLuint index, GLintptr offset, GLsizeiptr size)
{
    mBindingIndex = index;
    glBindBufferRange(mTarget, mBindingIndex, mId, offset, size);
}


// *** Private Methods *** //

void ArrayBufferObject::setCapacity(GLsizei capacity)
{
    mCapacity = capacity;
    mDataCache.resize(mCapacity);
    glNamedBufferData(mId, mCapacity, NULL, mCreateFlags);
}

GLsizei ArrayBufferObject::sizeOf(GLenum type)
{
    switch(type)
    {
        case GL_BYTE:
            return sizeof(GLbyte);
        case GL_INT:
            return sizeof(GLint);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_FLOAT:
            return sizeof(GLfloat);
    }
    
    return 0;
}


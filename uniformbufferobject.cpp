#include "uniformbufferobject.h"
#include "logger.hpp"


UniformBufferObject::UniformBufferObject(unsigned int requestedCapacity, GLvoid* data, std::size_t componentSize)
:
    mCapacity(requestedCapacity),
    mComponentSize(componentSize)
{
    glGenBuffers(1, &mId);
    glBindBuffer(mTarget, mId);
    glBufferData(mTarget, mCapacity, data, mCreateFlags);
    glBindBuffer(mTarget, 0);
    
    mDataCache.resize(requestedCapacity);
    if(data != nullptr)
        std::copy(static_cast<const glm::byte *>(data), static_cast<const glm::byte *>(data) + requestedCapacity, begin(mDataCache));
           
    mUpdateOffset = mDataCache.size();
    mUpdateSize = 0;    
}

UniformBufferObject::~UniformBufferObject()
{
    glDeleteBuffers(1, &mId);
}


// *** Public Methods *** //

void UniformBufferObject::dispose()
{
    glDeleteBuffers(1, &mId);    
}

void UniformBufferObject::bind()
{
    glBindBuffer(mTarget, mId);
}

void UniformBufferObject::unmap()
{
    glUnmapBuffer(mTarget);
}

glm::byte* UniformBufferObject::map(unsigned int requestedCapacity)
{
    glBindBuffer(mTarget, mId);
    if(mCapacity < requestedCapacity)
        setCapacity(requestedCapacity);

    mData = static_cast<glm::byte *>(glMapBufferRange(mTarget, 0, mCapacity, mMapFlags));

    return mData;
}

void UniformBufferObject::update(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
    std::copy(byteData, byteData + size, begin(mDataCache) + offset);    
    
    glNamedBufferSubData(mId, offset, size, data);
    
//     glBindBuffer(mTarget, mId);
//     glBufferSubData(mTarget, offset, size, data);
//     glBindBuffer(mTarget, 0);
}

void UniformBufferObject::updateCache(GLintptr offset, GLsizeiptr size, const GLvoid* data)
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
        
/*        mUpdateOffset = offset;
        
        if(mUpdateSize != 0)
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
//         auto distance = offset - mUpdateOffset;
// 
//         if((mUpdateSize - distance) > size)
//             mUpdateSize += distance;
//         else
//             mUpdateSize = distance + size;
    }
    else if (offset == mUpdateOffset && size > mUpdateSize)
        mUpdateSize = size;
}

void UniformBufferObject::updateGPU()
{
    if(mUpdateSize > 0)
    {
        glNamedBufferSubData(mId, mUpdateOffset * sizeof(glm::byte), mUpdateSize * sizeof(glm::byte), mDataCache.data() + mUpdateOffset);        
//         glBindBuffer(mTarget, mId);
//         glBufferSubData(mTarget, mUpdateOffset * sizeof(glm::byte), mUpdateSize * sizeof(glm::byte), mDataCache.data() + mUpdateOffset);
//         glBindBuffer(mTarget, 0);
        
        mUpdateSize = 0;
        mUpdateOffset = mDataCache.capacity();
    }
}


void UniformBufferObject::bindToIndexedBufferTarget(GLuint index, GLintptr offset, GLsizeiptr size)
{
    mBindingIndex = index;
    glBindBufferRange(mTarget, mBindingIndex, mId, offset, size);
}



// *** Private Methods *** //

void UniformBufferObject::setCapacity(int capacity)
{
    mCapacity = capacity;
    glBufferData(mTarget, mCapacity, NULL, mCreateFlags);
}

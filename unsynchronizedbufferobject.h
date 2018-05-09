#ifndef UNSYNCHRONIZEDBUFFEROBJECT_H
#define UNSYNCHRONIZEDBUFFEROBJECT_H

#include <bufferobject.h>

#include <GL/glew.h>

class UnsynchronizedBufferObject : public BufferObject
{
public:
    UnsynchronizedBufferObject(GLenum target, unsigned int requestedCapacity = 3);
    ~UnsynchronizedBufferObject();
    
    virtual void dispose();
    virtual void bind();
    virtual void unmap();
    virtual void *map(int requestedCapacity);
    
private:
    void setCapacity(int capacity);
    
    GLenum mTarget;
    GLuint mId;
    GLuint mCapacity;
    GLbitfield mMapFlags;
    GLbitfield mCreateFlags;
    void *mData;    
};

#endif // UNSYNCHRONIZEDBUFFEROBJECT_H

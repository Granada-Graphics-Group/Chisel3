#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#define GLM_FORCE_RADIANS
#include <glm/ext.hpp>


class BufferObject
{
public:
    ~BufferObject();
    virtual glm::byte* map(unsigned int requestedCapacity) = 0;
    virtual void unmap() = 0;
    virtual void bind() = 0;
    virtual void dispose() = 0;
};

#endif // BUFFEROBJECT_H

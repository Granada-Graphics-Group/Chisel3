#include "vertexarray.h"

VertexArray::VertexArray()
{
    glCreateVertexArrays(1, &mId);
}

VertexArray::~VertexArray()
{
    
}


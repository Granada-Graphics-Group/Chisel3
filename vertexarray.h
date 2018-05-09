#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include <GL/glew.h>

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();
    
private:
    GLuint mId;
};

#endif // VERTEXARRAY_H

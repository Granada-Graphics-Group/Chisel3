#ifndef QUAD_H
#define QUAD_H

#include "mesh.h"

class Quad : public Mesh
{
public:
    Quad(std::string name, glm::vec2 orig, glm::vec2 dimensions, glm::vec4 color = {0.5, 0.5, 0.5, 1.0});
    
    void changeDimensions(glm::vec2 dimensions);
    void resize(glm::vec2 orig, glm::vec2 dimensions);
};

#endif


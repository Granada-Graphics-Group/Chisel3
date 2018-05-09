#ifndef CONE_H
#define CONE_H

#include "cylinder.h"

class Cone : public Cylinder
{
public:
    Cone(std::string name, float radius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color = {0.5, 0.5, 0.5, 1.0});
    
    void resize(float radius, float height) { Cylinder::resize(radius, 0.0f, height); };
};

#endif

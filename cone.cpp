#include "cone.h"

Cone::Cone(std::string name, float radius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color)
:
    Cylinder(name, radius, 0, height, radialSlices, heightSteps, isOpen, color)
{
    
}


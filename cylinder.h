#ifndef CYLINDER_H
#define CYLINDER_H

#include "mesh.h"

class Cylinder : public Mesh
{
public:
    Cylinder(std::string name, float bottomRadius, float topRadius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color = {0.5, 0.5, 0.5, 1.0});

    void resize(float bottomRadius, float topRadius, float height);
    
private:
    void generateCap(bool isTop, uint32_t initIndex);
    
    float mBottomRadius;
    float mTopRadius;
    float mHeight; 
    uint32_t mRadialSlices;
    uint32_t mHeightSteps;
    bool mOpeness;
    glm::vec4 mColor;
};

#endif

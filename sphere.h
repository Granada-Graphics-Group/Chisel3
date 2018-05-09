#ifndef SPHERE_H
#define SPHERE_H

#include "mesh.h"

class Sphere : public Mesh
{
public:
    Sphere(std::string name, float radius, int resolution);
    
private:
    std::string mName;
    float mRadius;
    int mResolution;    
};

#endif

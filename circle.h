#ifndef CIRCLE_H
#define CIRCLE_H

#include "mesh.h"

class Circle : public Mesh
{
public:
    Circle(std::string name, glm::vec3 center, float radius, int slices);
    //~Circle();
    
    float radius(){ return mRadius; }
    float slices(){ return mSlices; }
    
    void changeDimensions(float radius, int slices);
    void changeRadius(float radius) { changeDimensions(radius, mSlices); };
    void changeSlices(int slices){ changeDimensions(mRadius, slices); } ;
    
private:
    glm::vec3 mCenter;
    float mRadius;
    int mSlices;
};

#endif // CIRCLE_H

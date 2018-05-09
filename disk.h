#ifndef DISK_H
#define DISK_H

#include "mesh.h"

class Disk : public Mesh
{
public:
    Disk(std::string name, float innerRadius, float outerRadius, int slices);
    ~Disk();
    
    float innerRadius() { return mInnerRadius; }
    float outerRadius() { return mOuterRadius; }
    float radiusWidth() { return mOuterRadius - mInnerRadius; }
    float slices() { return mSlices; }
    
    void changeDimensions(float innerRadius, float outerRadius, int slices);
    void changeRadius(float innerRadius, float outerRadius) { changeDimensions(innerRadius, outerRadius, mSlices); };
    void changeSlices(int slices){ changeDimensions(mInnerRadius, mOuterRadius, slices); } ;
    
private:
    glm::vec3 mCenter;
    float mInnerRadius;
    float mOuterRadius;
    int mSlices;    
};

#endif // DISK_H

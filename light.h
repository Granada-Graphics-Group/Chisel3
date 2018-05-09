#ifndef LIGHT_H
#define LIGHT_H

#include "cerealglm.h"

#include <string>
#include <glm/glm.hpp>
#include <cereal/types/string.hpp>

class Light
{
public:
    enum lightType
    {
        Point,
        Directional,
        Spot,
        Area,
        Volume
    };

    Light(){};
    Light(std::string name, lightType type, glm::mat4 matrix): mName(name), mType(type), mMatrix(matrix){}
    Light(glm::vec3 position);
    ~Light();
    
    std::string name() const { return mName; }
    lightType type() const { return mType; }
    const glm::mat4& matrix() const { return mMatrix; }
    const glm::vec3 position() const { return glm::vec3(mMatrix[3]); }
    
    void setName(std::string name) { mName = name; }

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(mName, mType, mPosition, mMatrix);
    }

private:
    std::string mName;
    lightType mType;
    glm::vec3 mPosition;
    glm::mat4 mMatrix;
};

#endif // LIGHT_H

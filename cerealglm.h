#ifndef CEREALGLM_H
#define CEREALGLM_H

#include <cereal/cereal.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cereal 
{
    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tvec2<T, P>& vector)
    {
        archive(vector.x, vector.y);
    }
            
    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tvec3<T, P>& vector)
    {
        archive(vector.x, vector.y, vector.z);
    }

    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tvec4<T, P>& vector)
    {
        archive(vector.x, vector.y, vector.z, vector.w);
    }
    
    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tquat<T, P>& quat)
    {
        archive(quat.w, quat.x, quat.y, quat.z);
    }
    
    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tmat3x3<T, P>& mat)
    {
        archive(mat[0][0], mat[0][1], mat[0][2],
                mat[1][0], mat[1][1], mat[1][2],
                mat[2][0], mat[2][1], mat[2][2]
               );
    }

    template<class Archive, class T, glm::precision P>
    void serialize(Archive& archive, glm::tmat4x4<T, P>& mat)
    {
        archive(mat[0][0], mat[0][1], mat[0][2], mat[0][3], 
                mat[1][0], mat[1][1], mat[1][2], mat[1][3], 
                mat[2][0], mat[2][1], mat[2][2], mat[2][3], 
                mat[3][0], mat[3][1], mat[3][2], mat[3][3] 
               );
    }
};

#endif

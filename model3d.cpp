#include "model3d.h"
#include "mesh.h"
#include "material.h"
#include "scene3d.h"

#include <glm/gtx/matrix_decompose.hpp>

#include <algorithm>
#include <string>
#include <iostream>


Model3D::Model3D(std::string name)
:
    mName(name),
    mMesh(nullptr)
{

}


Model3D::Model3D(std::string name, Mesh* mesh, std::vector< Material* > materials, glm::mat4 matrix)
:
    mName(name),
    mMesh(mesh),
    mMaterials(materials),
    mMatrix(matrix)    
{

}


Model3D::Model3D(Mesh* mesh)
:
    mMesh(mesh)
{
    generateBufferData();
}

Model3D::~Model3D()
{

}

// *** Public Methods *** //

std::array<int, 3> Model3D::sceneIndex(Scene3D* scene) const
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    return (search != end(mScenes)) ? std::array<int, 3>{mMesh->sceneIndex(scene), mMaterials[0]->sceneIndex(scene), (*search).second} : std::array<int, 3>{-1, -1, -1};
}

const glm::mat4 & Model3D::modelMatrix()
{
    if (mComposeMatrix) 
    {
        mMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::mat4_cast(mOrientation) * glm::scale(mScale);
        mComposeMatrix = false;
    }    
    return mMatrix;    
}

void Model3D::setOrientation(const glm::quat & orientation)
{
    mOrientation = orientation;
    mComposeMatrix = true;

    for (auto scene : mScenes)
        (scene.first)->setModelDirty(this);
}

void Model3D::setPosition(const glm::vec3 & position)
{
    mPosition = position;
    mComposeMatrix = true;

    for (auto scene : mScenes)
        (scene.first)->setModelDirty(this);
}

void Model3D::setScale(const glm::vec3 & scale)
{
    mScale = scale;
    mComposeMatrix = true;

    for (auto scene : mScenes)
        (scene.first)->setModelDirty(this);
}

void Model3D::setModelMatrix(glm::mat4 matrix)
{
    mMatrix = matrix;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation, translation, skew, perspective);

    mOrientation = glm::conjugate(rotation);
    mPosition = translation;
    mScale = scale;

    //mComposeMatrix = true;
    
    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);
}

void Model3D::lookAt(const glm::vec3& center)
{
    
}

void Model3D::lookAt(float centerX, float centerY, float centerZ)
{
    
}

void Model3D::lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
    
}

void Model3D::roll(float angle)
{
    glm::quat quat = glm::angleAxis(angle, mForward);

    mUp = glm::rotate(quat, mUp);
    mLeft = glm::rotate(quat, mLeft);

    mOrientation = quat * mOrientation;

    registerRotation();
    mComposeMatrix = true;
}

void Model3D::pitch(float angle)
{
    glm::quat quat = glm::angleAxis(angle, -1.0f * mLeft);

    mUp = glm::rotate(quat, mUp);
    mForward = glm::rotate(quat, mForward);

    mOrientation = quat * mOrientation;

    registerRotation();
    mComposeMatrix = true;    
}

void Model3D::yaw(float angle)
{
    glm::quat quat = glm::angleAxis(angle, mUp);

    mLeft = glm::rotate(quat, mLeft);
    mForward = glm::rotate(quat, mForward);

    mOrientation = quat * mOrientation;

    registerRotation();
    mComposeMatrix = true;    
}

void Model3D::rotate(float angle, const glm::vec3& axis)
{
    glm::vec3 nAxis = glm::normalize(axis);
    glm::quat quat = glm::angleAxis(angle, nAxis);

    mLeft = glm::rotate(quat, mLeft);
    mUp = glm::rotate(quat, mUp);
    mForward = glm::rotate(quat, mForward);

    mOrientation = quat * mOrientation;

    registerRotation();
    mComposeMatrix = true;

    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);    
}

void Model3D::rotateRelativeX(float angle)
{
    auto worldUp = glm::vec3(0.0, 1.0, 0.0);
    auto tempLeft = glm::normalize(glm::cross(mPosition, worldUp));
    //auto tempUp = glm::normalize(glm::cross(tempLeft, mEyePosition));

    rotate(angle, tempLeft);
}

void Model3D::rotateRelativeY(float angle)
{
    auto worldUp = glm::vec3(0.0, 1.0, 0.0);
    auto tempLeft = glm::normalize(glm::cross(mPosition, worldUp));
    auto tempUp = glm::normalize(glm::cross(tempLeft, mPosition));

    rotate(angle, tempUp);    
}

void Model3D::rotateRelativeZ(float angle)
{
    
}

void Model3D::translateOnAxis(const glm::vec3& axis, float distance)
{
    auto rotationMatrix = glm::toMat3(mOrientation);
    auto newAxis = rotationMatrix * axis;
    newAxis *= distance;
    mPosition += newAxis;
    
    mComposeMatrix = true;
    
    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);     
}


void Model3D::translate(float x, float y, float z)
{
    mPosition += glm::vec3(x, y, z);

    mComposeMatrix = true;

    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);    
}

void Model3D::translate(const glm::vec3& v)
{
    mPosition += v;
    
    mComposeMatrix = true;
    
    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);     
}

void Model3D::translateRelative(float left, float up, float forward)
{
    auto worldUp = glm::vec3(0.0, 1.0, 0.0);
    auto tempLeft = glm::normalize(glm::cross(mPosition, worldUp));
    auto tempUp = glm::normalize(glm::cross(tempLeft, mPosition));
    
    mPosition += left * tempLeft;
    mPosition += up * tempUp;
    mPosition += forward * mForward;

    mComposeMatrix = true;

    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);    
}

void Model3D::translateRelative(const glm::vec3& v)
{
    translateRelative(v.x, v.y, v.z);
}

void Model3D::resetRotation()
{
    mForward = glm::vec3(0.0, 0.0, 1.0);
    mUp = glm::vec3(0.0, 1.0, 0.0);
    mLeft = glm::normalize(glm::cross(mUp, mForward));
    
    
    glm::mat3 m;
    m[0] = 1.0f * mLeft;                // first column, representing new x-axis orientation
    m[1] = mUp;                         // second column, representing new y-axis orientation
    m[2] = 1.0f * mForward;             // third column, representing new z-axis orientation
    mOrientation = glm::quat_cast(m);
    registerRotation();
    
    mComposeMatrix = true;

    for(auto scene : mScenes)
        (scene.first)->setModelDirty(this);    
}

void Model3D::resetPosition()
{
    
}

void Model3D::applyTransform(glm::mat4 transform, int buffer, bool reset)
{
    const auto meshBuffer = mesh()->vertexBuffer();
    auto elements = mesh()->vertexBuffer().size()/sizeof(float);
    
    auto floatbuffer = reinterpret_cast<const float *>(meshBuffer.data());
    std::vector<float> result;
    
    for(auto i = 0; i < elements; i += 3)
    {
        glm::vec4 vertex(floatbuffer[i], floatbuffer[i + 1], floatbuffer[i + 2], 1.0);        
        vertex = transform * vertex;
        result.push_back(vertex.x);
        result.push_back(vertex.y);
        result.push_back(vertex.z);        
    }
    
    mesh()->updateData(buffer, 0, mesh()->vertexBuffer().size(), result.data());
    
    if(reset) mMatrix = glm::mat4(1.0);
}


bool Model3D::addScene(Scene3D* scene, int index)
{
    if(std::find(begin(mScenes), end(mScenes), std::pair<Scene3D*, int>(scene, index)) == end(mScenes))
    {
        mScenes.push_back({scene, index});
        return true;
    }
    else
        return false;
}


bool Model3D::removeScene(Scene3D* scene)
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    if(search != end(mScenes))
    {
        mScenes.erase(search);
        return true;
    }
    else
        return false;
}


void Model3D::draw()
{
    glBindVertexArray(mMesh->VAO());
    glDrawElements(GL_TRIANGLES, mMesh->elementCount(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}





// *** Private Methods *** //

void Model3D::generateBufferData()
{
/*   auto vertices = mMesh->vertices();
   
   for(auto vertex: vertices)
   {
       auto position = vertex->position();
       auto normal = vertex->normal();
       auto uv = vertex->textureCoord();
       
       mVertexBufferData.insert(end(mVertexBufferData), &position[0], &position[0] + 3);
       mNormalBufferData.insert(end(mNormalBufferData), &position[0], &position[0] + 3);
       mUVBufferData.insert(end(mUVBufferData), &position[0], &position[0] + 2);
   } */
}

void Model3D::registerRotation()
{
    mRotationHitCount++;

    if (mRotationHitCount > mRotationHitCountMax) 
    {
        mRotationHitCount = 0;
        normalizeCamera();
    }    
}

void Model3D::normalizeCamera()
{
    mLeft = glm::normalize(mLeft);
    mUp = glm::normalize(mUp);
    mForward = glm::normalize(mForward);
    mOrientation = glm::normalize(mOrientation);

    // Assuming forward 'f' is correct
    mLeft = glm::cross(mUp, mForward);
    mUp = glm::cross(mForward, mLeft);    
}

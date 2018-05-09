#ifndef CAMERA_H
#define CAMERA_H

#include "cerealglm.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <GL/glew.h>
#include <string>

#include <cereal/types/string.hpp>

class Camera
{
public:
    Camera();
    Camera(std::string name);
    Camera(std::string name, glm::mat4 projectionMatrix) : Camera(name, projectionMatrix, glm::mat4(1.0)) {};
    Camera(std::string name, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
    Camera(float left, float right, float bottom, float top, float zNear, float zFar);
    Camera(float fieldOfViewY, float aspectRatio, float zNear, float zFar);

    // Setters
    void setPosition(float x, float y, float z);
    void setPosition(const glm::vec3 &position);
    void setForward(const glm::vec3 &forward);
    void setUp(const glm::vec3 &up);
    void setOrientation(glm::quat orientation);
    void setTarget(const glm::vec3 &target);
    void setViewMatrix(glm::mat4 modelView) { mViewMatrix = modelView; recalcViewMatrix = false;}
    
    void setProjectionMatrix(glm::mat4 projection) { mProjectionMatrix = projection; }
    void setAspectWidth(GLdouble width) { mAspectRatio.x = width; }
    void setAspectHeight(GLdouble height) { mAspectRatio.y = height; }
    void setAspectRatio(GLdouble aspectRatio) { mAspectRatio.z = aspectRatio; }
    void setFieldOfViewX(GLdouble fovX) { mFieldofView.x = fovX; }
    void setFieldOfViewY(GLdouble fovY) { mFieldofView.y = fovY; }
    void setFieldOfView(GLdouble fov) { mFieldofView.z = fov; }
    void setFocalLength(GLdouble focalLength) { mFocalLength = focalLength; }
    void setNearPlane(GLdouble nearPlane) { mNearPlane = nearPlane; }
    void setFarPlane(GLdouble farPlane) { mFarPlane = farPlane; }

    // Getters
    std::string name() const;
    glm::vec3 position() const;
    glm::vec3 leftDirection() const;
    glm::vec3 upDirection() const;
    glm::vec3 forwardDirection() const;
    glm::quat orientation() const;
    
    const glm::mat4& viewMatrix();
    
    const glm::mat4& projectionMatrix() const;    
    double aspectWidth() const { return mAspectRatio.x; }
    double aspectHeight() const { return mAspectRatio.y; }
    double aspectRatio() const { return mAspectRatio.z; }    
    double fieldOfViewX() const { return mFieldofView.x; }
    double fieldOfViewY() const { return mFieldofView.y; }
    double fieldOfView() const { return mFieldofView.z; }    
    double focalLength() const { return mFocalLength; }
    double nearPlane() const { return mNearPlane; }
    double farPlane() const { return mFarPlane; }

    // Actions
    void lookAt(const glm::vec3 & center);
    void lookAt(float centerX, float centerY, float centerZ);
    void lookAt(const glm::vec3 & eye,
                const glm::vec3 & center,
                const glm::vec3 & up);
    void roll(float angle);
    void pitch(float angle);
    void yaw(float angle);
    void rotate(float angle, const glm::vec3 & axis);
    void translate(float x, float y, float z);
    void translate(const glm::vec3 &v);
    void translateRelative(float left, float up, float forward);
    void translateRelative(const glm::vec3 &v);
    void resetRotation();
    void resetPosition();
    mutable bool recalcViewMatrix;
    
    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(mName, mProjectionMatrix, mFocalLength, mFieldofView, mAspectRatio, mNearPlane, mFarPlane, mViewMatrix, mEyePosition, mOrientation); 
    }
    
private:
    void registerRotation();
    void normalizeCamera();
    
    std::string mName;
    
    glm::vec3 mEyePosition; // Location of camera in world coordinates.
    glm::quat mOrientation; // Orientation of camera basis vectors specified in world coordinates.
    glm::vec3 mTarget;

    glm::vec3 mLeft; // Camera's left direction vector, given in world coordinates.
    glm::vec3 mUp; // Camera's up direction vector, given in world coordinates.
    glm::vec3 mForward; // Camera's forward direction vector, given in world coordinates.

    //mutable bool recalcViewMatrix;
    glm::mat4 mViewMatrix;
    

    
    glm::mat4 mProjectionMatrix;
    GLdouble mFocalLength;
    glm::dvec3 mFieldofView;
    glm::dvec3 mAspectRatio;
    GLdouble mNearPlane;
    GLdouble mFarPlane;
    
    

    unsigned short mRotationHitCount;

    // Normalize Camera vectors after rotating this many times.
    static const unsigned short mRotationHitCountMax = 1000;
};

#endif // CAMERA_H

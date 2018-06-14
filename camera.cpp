#include "camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

Camera::Camera()
: 
    recalcViewMatrix(true),
    mRotationHitCount(0),
    mLeft(-1.0, 0.0, 0.0),
    mUp(0.0, 1.0, 0.0),
    mForward(0.0, 0.0, -1.0),
    mEyePosition(0.0, 0.0, 0.0),
    mViewMatrix(glm::mat4(1.0))
{

}

Camera::Camera(std::string name)
:
    mName(name),
    recalcViewMatrix(true),
    mRotationHitCount(0),
    mLeft(-1.0, 0.0, 0.0),
    mUp(0.0, 1.0, 0.0),
    mForward(0.0, 0.0, -1.0),
    mEyePosition(0.0, 0.0, 0.0),
    mViewMatrix(glm::mat4(1.0))    
{

}


Camera::Camera(std::string name, glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
:
    mName(name),
    mProjectionMatrix(projectionMatrix),
    mViewMatrix(viewMatrix),
    recalcViewMatrix(false)
{

}


//----------------------------------------------------------------------------------------
/**
* Constructs an orthographic projected \c Camera.
*
* @note If zNear or zFar are negative, corresponding z clipping planes
* are considered behind the viewer.
*
* @param left - specify location of left clipping plane.
* @param right - specify location of right clipping plane.
* @param bottom - specify location of bottom clipping plane.
* @param top - specify location of top clipping plane.
* @param zNear - distance to near z clipping plane.
* @param zFar - distance to far z clipping plane.
*/
Camera::Camera(float left, float right, float bottom, float top, float zNear, float zFar)
: //Frustum(left, right, bottom, top, zNear, zFar),
    recalcViewMatrix(true),
    mRotationHitCount(0),
    mLeft(-1.0, 0.0, 0.0),
    mUp(0.0, 1.0, 0.0),
    mForward(0.0, 0.0, -1.0),
    mEyePosition(0.0, 0.0, 0.0)
{

}

//----------------------------------------------------------------------------------------
/**
* Constructs a perspective projected \c Camera.
*
* @note Depth buffer precision is affected by the values specified for
* zNear and zFar. The greater the ratio of zFar to zNear is, the less
* effective the depth buffer will be at distinguishing between surfaces
* that are near each other. If r = zFar zNear roughly log2(r) bits
* of depth buffer precision are lost. Because r approaches infinity as
* zNear approaches 0, zNear must never be set to 0.
*
* @param fieldOfViewY
* @param aspectRatio
* @param zNear - distance to near z clipping plane (always positive).
* @param zFar - distance to far z clipping plane (always positive).
*/
Camera::Camera(float fieldOfViewY, float aspectRatio, float zNear, float zFar)
: //Frustum(fieldOfViewY, aspectRatio, zNear, zFar),
    recalcViewMatrix(true),
    mRotationHitCount(0),
    mLeft(-1.0, 0.0, 0.0),
    mUp(0.0, 1.0, 0.0),
    mForward(0.0, 0.0, -1.0),
    mEyePosition(0.0, 0.0, 0.0)
{
    
}

//----------------------------------------------------------------------------------------
/**
* Sets the world position of the \c Camera.
* @param x
* @param y
* @param z
*/
void Camera::setPosition(float x, float y, float z) 
{
    mEyePosition.x = x;
    mEyePosition.y = y;
    mEyePosition.z = z;

    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Sets the world position of the \c Camera.
* @param v
*/
void Camera::setPosition(const glm::vec3& position) 
{
    setPosition(position.x, position.y, position.z);
}

void Camera::setForward(const glm::vec3& forward)
{
    mForward = forward;
    recalcViewMatrix = true;
}

void Camera::setUp(const glm::vec3& up)
{
    mUp = up;
    recalcViewMatrix = true;
}

void Camera::setOrientation(glm::quat orientation)
{
    mOrientation = orientation;
    recalcViewMatrix = true;
}

void Camera::setTarget(const glm::vec3& target)
{
    mTarget = target;
}


std::string Camera::name() const
{
    return mName;
}


//----------------------------------------------------------------------------------------
/**
* @return the world position of the \c Camera.
*/
glm::vec3 Camera::position() const 
{
    return mEyePosition;
}


//----------------------------------------------------------------------------------------
/**
* @return the \c Camera's left direction vector given in world space coordinates.
*/
glm::vec3 Camera::leftDirection() const 
{
    return mLeft;
}

//----------------------------------------------------------------------------------------
/**
* @return the \c Camera's up direction vector given in world space coordinates.
*/
glm::vec3 Camera::upDirection() const 
{
    return mUp;
}

//----------------------------------------------------------------------------------------
/**
* @return the \c Camera's forward direction vector given in world space coordinates.
*/
glm::vec3 Camera::forwardDirection() const 
{
    return mForward;
}

//----------------------------------------------------------------------------------------
/**
* @note Orientation of the Camera is given by a float glm::quaternion of the form
* \c (cos(theta/2), sin(theta/2) * u), where the axis of rotation \c u is given in
* world space coordinates.
*
* @return the \c Camera's orientation in the form of a float glm::quaternion.
*/
glm::quat Camera::orientation() const 
{
    return mOrientation;
}

const glm::mat4& Camera::projectionMatrix() const
{
    return mProjectionMatrix;
}


//----------------------------------------------------------------------------------------
/**
* @return a 4x4 view matrix representing the 'Camera' object's view transformation.
*/
const glm::mat4& Camera::viewMatrix()
{
    if (recalcViewMatrix) 
    {
        mViewMatrix = glm::translate(glm::mat4(1.0f), -1.0f * mEyePosition);
        mViewMatrix = mViewMatrix * glm::mat4_cast(mOrientation);

        recalcViewMatrix = false;
    }

    return mViewMatrix;
}

//----------------------------------------------------------------------------------------
void Camera::normalizeCamera() 
{
    mLeft = glm::normalize(mLeft);
    mUp = glm::normalize(mUp);
    mForward = glm::normalize(mForward);
    mOrientation = glm::normalize(mOrientation);

    // Assuming forward 'f' is correct
    mLeft = glm::cross(mUp, mForward);
    mUp = glm::cross(mForward, mLeft);
}

//----------------------------------------------------------------------------------------
void Camera::registerRotation() 
{
    mRotationHitCount++;

    if (mRotationHitCount > mRotationHitCountMax) 
    {
        mRotationHitCount = 0;
        normalizeCamera();
    }
}

//----------------------------------------------------------------------------------------
/**
* Rotates \c Camera about its local negative z-axis (forward direction)
* by \c angle radians.
*
* @note Rotation is counter-clockwise if \c angle > 0, and clockwise if
* \c angle is < 0.
*
* @param angle - rotation angle in radians.
*/
void Camera::roll(float angle) 
{
    glm::quat q = glm::angleAxis(angle, mForward);

    mUp = glm::rotate(q, mUp);
    mLeft = glm::rotate(q, mLeft);

    mOrientation = q * mOrientation;

    registerRotation();
    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Rotates \c Camera about its local x (right direction) axis by \c angle
* radians.
*
* @note Rotation is counter-clockwise if \c angle > 0, and clockwise if
* \c angle is < 0.
*
* @param angle - rotation angle in radians.
*/
void Camera::pitch(float angle) 
{
    glm::quat q = glm::angleAxis(angle, -1.0f * mLeft);

    mUp = glm::rotate(q, mUp);
    mForward = glm::rotate(q, mForward);

    mOrientation = q * mOrientation;

    registerRotation();
    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Rotates \c Camera about its local y (up direction) axis by \c angle
* radians.
*
* @note Rotation is counter-clockwise if \c angle > 0, and clockwise if
* \c angle is < 0.
*
* @param angle - rotation angle in radians.
*/
void Camera::yaw(float angle) 
{
    glm::quat q = glm::angleAxis(angle, mUp);

    mLeft = glm::rotate(q, mLeft);
    mForward = glm::rotate(q, mForward);

    mOrientation = q * mOrientation;

    registerRotation();
    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Rotates \c Camera by \c angle radians about \c axis whose components are expressed using
* the \c Camera's local coordinate system.
*
* @note Counter-clockwise rotation for angle > 0, and clockwise rotation otherwise.
*
* @param angle
* @param axis
*/
void Camera::rotate(float angle, const glm::vec3 & axis) 
{
    glm::vec3 n = glm::normalize(axis);
    glm::quat q = glm::angleAxis(angle, n);

    mLeft = glm::rotate(q, mLeft);
    mUp = glm::rotate(q, mUp);
    mForward = glm::rotate(q, mForward);

    mOrientation = q * mOrientation;

    registerRotation();
    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Translates the \c Camera with respect to the world coordinate system.
*
* @param x
* @param y
* @param z
*/
void Camera::translate(float x, float y, float z) 
{
    mEyePosition.x += x;
    mEyePosition.y += y;
    mEyePosition.z += z;

    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Translates the \c Camera with respect to the world coordinate system.
*
* @param v
*/
void Camera::translate(const glm::vec3& v) 
{
    translate(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------
/**
* Translates the \c Camera relative to its locate coordinate system.
*
* @param left - translation along the \c Camera's left direction.
* @param up - translation along the \c Camera's up direction.
* @param forward - translation along the \c Camera's forward direction.
*/
void Camera::translateRelative(float left, float up, float forward) 
{
    auto worldUp = glm::vec3(0.0, 1.0, 0.0);
    auto tempLeft = glm::normalize(glm::cross(mEyePosition, worldUp));
    auto tempUp = glm::normalize(glm::cross(tempLeft, mEyePosition));
    
    mEyePosition += left * tempLeft;
    mEyePosition += up * tempUp;
    mEyePosition += forward * mForward;

    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
/**
* Translates the \c Camera relative to its locate coordinate system.
*
* v.x - translation along the \c Camera's left direction.
* v.y - translation along the \c Camera's up direction.
* v.z - translation along the \c Camera's forward direction.
*
* @param v
*/
void Camera::translateRelative(const glm::vec3& v) 
{
    translateRelative(v.x, v.y, v.z);
}

void Camera::resetRotation()
{
    mForward = glm::vec3(0.0, 0.0, 1.0);
    mUp = glm::vec3(0.0, 1.0, 0.0);
    mLeft = glm::normalize(glm::cross(mUp, mForward));
    
    
    glm::mat3 m;
    m[0] = 1.0f * mLeft; // first column, representing new x-axis orientation
    m[1] = mUp; // second column, representing new y-axis orientation
    m[2] = 1.0f * mForward; // third column, representing new z-axis orientation
    mOrientation = glm::quat_cast(m);
    registerRotation();
    
    recalcViewMatrix = true;
}


//----------------------------------------------------------------------------------------
void Camera::lookAt(const glm::vec3 & center) 
{
    this->lookAt(center.x, center.y, center.z);
}

//----------------------------------------------------------------------------------------
/*
* TODO (Dustin) Make lookAt more robust.
* A better algorithm may be to compute the transformation or glm::quaternion
* that takes previous f to new f, then rotate u and l by it. This will handle the
* case when new f = prior u.
*/
void Camera::lookAt(float centerX, float centerY, float centerZ) 
{
    // f = center - eye.
    mForward.x = centerX - mEyePosition.x;
    mForward.y = centerY - mEyePosition.y;
    mForward.z = centerZ - mEyePosition.z;
    mForward = glm::normalize(mForward);

    // XXX (Dustin) It's possible that f = u here.

    // The following projects u onto the plane defined by the point eyePosition,
    // and the normal f. The goal is to rotate u so that it is orthogonal to f,
    // while attempting to keep u's orientation close to its previous direction.
    {
        // Borrow l vector for calculation, so we don't have to allocate a
        // new vector.
        mLeft = mEyePosition + mUp;

        float t = -1.0f * glm::dot(mForward, mUp);

        // Move point l in the normal direction, f, by t units so that it is
        // on the plane.
        mLeft.x += t * mForward.x;
        mLeft.y += t * mForward.y;
        mLeft.z += t * mForward.z;

        mUp = mLeft - mEyePosition;
        mUp = glm::normalize(mUp);
    }

    // Update l vector given new f and u vectors.
    mLeft = glm::cross(mUp, mForward);

    // If f and u are no longer orthogonal, make them so.
    if (glm::dot(mForward, mUp) > 1e-7f) {
        mUp = glm::cross(mForward, mLeft);
    }

    glm::mat3 m;
    m[0] = -1.0f * mLeft; // Camera's local x axis
    m[1] = mUp; // Camera's local y axis
    m[2] = -1.0f * mForward; // Camera's local z axis
    mOrientation = glm::toQuat(m);

    registerRotation();
    recalcViewMatrix = true;
}

//----------------------------------------------------------------------------------------
void Camera::lookAt(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up) 
{
    mEyePosition = eye;

    // Orient Camera basis vectors.
    mForward = glm::normalize(center - eye);
    mLeft = glm::normalize(glm::cross(up, mForward));
    mUp = up;

    // Compute orientation from 3x3 change of basis matrix whose columns are the
    // world basis vectors given in Camera space coordinates.
    glm::mat3 m;
    m[0] = -1.0f * mLeft; // first column, representing new x-axis orientation
    m[1] = up; // second column, representing new y-axis orientation
    m[2] = -1.0f * mForward; // third column, representing new z-axis orientation
    mOrientation = glm::quat_cast(m);

    registerRotation();
    recalcViewMatrix = true;
}



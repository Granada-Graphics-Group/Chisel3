#ifndef MODEL3D_H
#define MODEL3D_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <array>
#include <string>

class Mesh;
class Material;
class Scene3D;

class Model3D
{

public:
    Model3D() : Model3D("") {};
    Model3D(std::string name);
    Model3D(std::string name, Mesh* mesh) : Model3D(name, mesh, {}, glm::mat4(1.0)) {};
    Model3D(std::string name, Mesh* mesh, glm::mat4 matrix) : Model3D(name, mesh, {}, matrix) {};
    Model3D(std::string name, Mesh* mesh, std::vector<Material* > materials): Model3D(name, mesh, materials, glm::mat4(1.0)) {};
    Model3D(std::string name, Mesh* mesh, std::vector<Material* > materials, glm::mat4 matrix);
    Model3D(Mesh *mesh);
    ~Model3D();
    
    std::string name() const { return mName; }
    Mesh* mesh() const { return mMesh; }
    std::vector<Material* > materials() const { return mMaterials; }
    Material* material(unsigned int index) const { if (index < mMaterials.size()) return mMaterials[index]; else return nullptr; }
    std::array<int, 3> sceneIndex(Scene3D* scene) const;
    
    glm::quat orientation() const { return mOrientation; }
    glm::vec3 position() const { return mPosition; }
    glm::vec3 scale() const;
    const glm::mat4& modelMatrix();
    
    void setMesh(Mesh *mesh){ mMesh = mesh; }    
    void setModelMatrix(glm::mat4 matrix);
    void insertMaterial(Material *material){ mMaterials.push_back(material); };
    
    void lookAt(const glm::vec3 & center);
    void lookAt(float centerX, float centerY, float centerZ);
    void lookAt(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up);
    void roll(float angle);
    void pitch(float angle);
    void yaw(float angle);
    void rotate(float angle, const glm::vec3 & axis);
    void rotateRelativeX(float angle);
    void rotateRelativeY(float angle);
    void rotateRelativeZ(float angle);
    void translateOnAxis(const glm::vec3 &axis, float distance);
    void translate(float x, float y, float z);
    void translate(const glm::vec3 &v);
    void translateRelative(float left, float up, float forward);
    void translateRelative(const glm::vec3 &v);
    void resetRotation();
    void resetPosition();  
    void applyTransform(glm::mat4 transform, int buffer, bool reset);
    
    bool addScene(Scene3D *scene, int index);
    bool removeScene(Scene3D *scene);
            
    void draw();
        
private:
    void generateBufferData();
    void registerRotation();
    void normalizeCamera();
    
    std::string mName;    
    Mesh* mMesh;
    std::vector<Material*> mMaterials;
    std::vector< std::pair<Scene3D*, int > > mScenes;
    
    glm::mat4 mMatrix;
    
    glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};    // Location in world coordinates.
    glm::quat mOrientation = glm::quat_cast(glm::mat4(1.0));                      // Orientation of basis vectors specified in world coordinates.
    glm::vec3 mTarget;

    glm::vec3 mLeft = {-1.0f, 0.0f, 0.0f};       // Model's left direction vector, given in world coordinates.
    glm::vec3 mUp = {0.0f, 1.0f, 0.0f};          // Model's up direction vector, given in world coordinates.
    glm::vec3 mForward = {0.0f, 0.0f, 1.0f};     // Model's forward direction vector, given in world coordinates.
    
    mutable bool mComposeMatrix = false;
    
    unsigned short mRotationHitCount = 0;
    // Normalize Camera vectors after rotating this many times.
    static const unsigned short mRotationHitCountMax = 1000;    
};

#endif // MODEL3D_H

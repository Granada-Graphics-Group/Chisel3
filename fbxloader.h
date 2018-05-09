#ifndef FBXLOADER_H
#define FBXLOADER_H

#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif

//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>

#include "fileloader.h"
#include "texture.h"
#include "material.h"

#include <map>
#include <string>
#include <GL/glew.h>
#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

class ResourceManager;
class Scene3D;
class Model3D;
class Mesh;
class Camera;
class Light;

class FBXLoader : public FileLoader
{
public:
    FBXLoader(ResourceManager *manager);
    FBXLoader(ResourceManager *manager, Scene3D *scene, std::string path);    
    ~FBXLoader();
    void loadScene(Scene3D *scene, std::string path);
protected:
    virtual void extractWordsFromNextLine(std::ifstream& file, std::vector< std::string, std::allocator< void > >& words, const std::string& delimiter);
    virtual void extractWordsFromLine(std::ifstream& file, std::string& line, std::vector< std::string, std::allocator< void > >& words, const std::string& delimiter);
    virtual void extractWords(std::vector< std::string, std::allocator< void > >& words, std::string& line, const std::string& delimiter);
    
private:
    FbxAMatrix getGlobalPosition(FbxNode* node, const FbxTime& time, FbxPose* pose = nullptr, FbxAMatrix* parentGlobalPosition = nullptr);
    FbxAMatrix getPoseMatrix(FbxPose* pose, int nodeIndex);
    FbxAMatrix getGeometryOffset(FbxNode* node);
            
    void extractNode(FbxNode *node);
    void extractTransform(FbxNode* node, Model3D *model);
    void extractMesh(FbxMesh* fbxMesh, Mesh* mesh);
    void extractCamera(FbxCamera* fbxCamera, Camera* camera);
    void extractLights();
    void extractMaterial(const FbxSurfaceMaterial* fbxMaterial, Material* material);
    ColorChannel getMaterialProperty(const FbxSurfaceMaterial* material, const char* propertyName, const char* factorPropertyName);
    Texture* getMaterialPropertyTexture(const FbxSurfaceMaterial* material, const char* propertyName);
    
    struct SubMesh
    {
        unsigned int IndexOffset  = 0;
        unsigned int TriangleCount = 0;
    };

    FbxArray<SubMesh*> mSubMeshes;
    bool mHasColor = false;
    bool mHasNormal = false;
    bool mHasUV = false;
    bool mAllByControlPoint = false; // Save data in VBO by control point or by polygon vertex.

    //std::map <std::string, std::vector<Model3D *> > mGeometryInstances;
    //std::map <std::string, std::vector<Model3D *> > mMaterialInstances;
    
    ResourceManager* mResourceManager = nullptr;
    FbxManager* mManager = nullptr;
    FbxScene* mFbxScene = nullptr;
    FbxImporter* mImporter = nullptr;
    
    Scene3D* mScene3D = nullptr;
    
    unsigned int counter = 0;
};

#endif // FBXLOADER_H

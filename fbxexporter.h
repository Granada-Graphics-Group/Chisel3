#ifndef FBXEXPORTER_H
#define FBXEXPORTER_H

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <string>
#include <vector>
#include <map>

class ResourceManager;
class Scene3D;
class Camera;
class Model3D;

class FBXExporter
{
public:
    FBXExporter();
    ~FBXExporter();
    
    void exportSceneToFile(std::string filepath, Scene3D* scene, bool exportCamera);
    void exportModelToFile(std::string filepath, Model3D* model, const std::map<std::string, std::vector<uint32_t>>& segmentation, Camera* camera, bool exportCamera);
        
private:
    void populateFBXScene(FbxScene* fbxScene, Model3D* model, Camera* camera);
    void populateFBXScene(FbxScene* fbxScene, Model3D* model, const std::map<std::string, std::vector<uint32_t>>& segmentation, Camera* camera);
	void addCamera(fbxsdk::FbxScene* fbxScene, Camera* camera);
    
    FbxManager* mManager = nullptr;
    FbxScene* mFbxScene = nullptr;  
};


#endif

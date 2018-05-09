#ifndef SCENE3D_H
#define SCENE3D_H

#include "camera.h"
#include "light.h"

#include <vector>
#include <array>
#include <set>
#include <map>

class Mesh;
class Material;
class Model3D;
class GLRenderer;

typedef struct 
{
    unsigned int  mCount;
    unsigned int  mInstanceCount;
    unsigned int  mFirstIndex;
    unsigned int  mBaseVertex;
    unsigned int  mBaseInstance;
} DrawElementsIndirectCommand;


struct MeshOffsets
{
    unsigned int attrib;
    unsigned int index;
    unsigned int instance;
    unsigned int command;
    std::map<int, unsigned int> commandPerMaterial;
};

typedef std::map <std::string, std::vector<Model3D* > > innerMap;
typedef std::map <int, std::vector<Model3D*>> innerMap2;

class Scene3D
{
public:
    Scene3D(std::string name) : Scene3D(name, {}) {};
    Scene3D(std::string name, std::vector<Camera* > cameras) : Scene3D(name, cameras, {}) {};
    Scene3D(std::string name, std::vector<Camera* > cameras, std::vector<Model3D* > models) : Scene3D(name, cameras, models, {}) {};
    Scene3D(std::string name, std::vector<Camera* > cameras, std::vector<Model3D* > models, std::vector<Light* > lights);
    Scene3D(const Scene3D& other);
    ~Scene3D();
    
    int index() const { return mIndex; }
    std::string name() const { return mName; }
    Camera* camera() { return mCurrentCamera; }
    
    void setIndex(int index){ mIndex = index; }
    void setName(std::string name){ mName = name;}
    void updateCamera(Camera* camera){ mCurrentCamera = camera; }
    
    bool insertMesh(Mesh* mesh);
    bool insertMaterial(Material* material);
    bool insertModel(Model3D* model);
    void insertLight(Light* light) { mLights.push_back(light); }
    bool insertCamera(Camera* camera, bool current = true);
    void insertGeometryInstance(std::string mesh, Model3D* model);
    void insertMaterialInstance(std::string material, Model3D* model);
    
    void clearScene();

    Mesh* findMesh(std::string meshName) const;
    Material* findMaterial(std::string materialName) const;
    Model3D* findModel3D(std::string modelName) const;
    Light* findLight(std::string lightName) const;
    Camera* findCamera(std::string cameraName) const;
        
    std::vector<Mesh* > meshes() const { return mMeshes; }
    std::vector<Material* > materials() const { return mMaterials; }
    std::vector<Model3D* > models() const { return mModels; }
    Model3D* previousModel(const std::array<int, 3>& index) const;
    const std::vector<Light* >& lights() const { return mLights; }
    std::vector<Camera* > cameras() const { return mCameras; }
    std::map <std::string, std::vector<Model3D* > > modelsByGeometry() const { return mMeshInstances; }
    std::map <std::string, innerMap > modelsByGeometry2() const { return mMeshInstances2; }
    std::map <int, innerMap2> modelsPerMeshPerMaterial() const { return mModelsPerMeshPerMaterial; }
    std::map <std::string, std::vector<Model3D* > > modelsByMaterial() const { return mMaterialInstances; }
    std::map <std::string, innerMap > modelsByMaterial2() const { return mMeshInstancesPerMaterial; }
    std::map <int, innerMap2> modelsPerMaterialPerMesh() const { return mModelsPerMaterialPerMesh; }
    
    bool meshCountChanged() const { return mMeshCountChanged; }
    bool modelCountChanged() const { return mModelCountChanged; }
    bool viewCameraNeedUpdate() const { return mViewCameraNeedUpdate; }
    bool projCameraNeedUpdate() const { return mProjCameraNeedUpdate; }
    bool lightsNeedUpdate() const { return mLightsNeedUpdate; }
    bool meshesNeedUpdate() const { return mMeshesNeedUpdate; }
    bool modelsNeedUpdate() const { return mModelsNeedUpdate; }
    bool texturesNeedUpdate() const { return mTexturesNeedUpdate; }
    
    void setMeshCountChanged(bool value) { mMeshCountChanged = value; }
    void setModelCountChanged(bool value) { mModelCountChanged = value; }
    void setViewCameraNeedUpdate(bool value) { mViewCameraNeedUpdate = value; }
    void setProjCameraNeedUpdate(bool value) { mProjCameraNeedUpdate = value; }
    void setLightsNeedUpdate(bool value) { mLightsNeedUpdate = value; }
    void setMeshesNeedUpdate(bool value) { mMeshesNeedUpdate = value; }
    void setModelsNeedUpdate(bool value) { mModelsNeedUpdate = value; }
    void setTexturesNeedUpdate(bool value) { mTexturesNeedUpdate = value; }    

    const std::vector<Mesh *>& dirtyMeshes() const { return mDirtyMeshes; }
    const std::vector<Mesh *> dirtyMeshes2() const { std::vector<Mesh *> temp; for(auto idx : mDirtyMeshes2) temp.push_back(mMeshes[idx]); return temp;  }
    void setMeshDirty(Mesh* mesh);
    void setMeshesDirtyAfter(Mesh* mesh);
    void setMeshDirty(std::string meshName);
    void clearDirtyMeshes() { mDirtyMeshes.clear(); mDirtyMeshes2.clear(); mMeshesNeedUpdate = false; }
    
    const std::vector<Model3D*>& dirtyModels() const { return mDirtyModels; }
    const std::vector<Model3D*> dirtyModels2() const;
    void setModelDirty(Model3D* model);
    void setModelDirtyFrom(Model3D* model);
    //void setModelDirty(unsigned int index);
    void setModelDirty(std::string modelName);
    void clearDirtyModels() { mDirtyModels.clear(); mDirtyModels2.clear(); mModelsNeedUpdate = false; }
    
    const std::set<int>& materialsAffectedByDirtyModels() const { return mMaterialsAffectedByDirtyModels; }
    void clearMaterialsAffectedByDirtyModels() { mMaterialsAffectedByDirtyModels.clear(); }
    
    void setFullUpdate();
    void invalidateScene(){ invalidateSceneFromMesh(0); }
    void invalidateSceneFromMesh(unsigned int index);
    
    void rotate(float angle, const glm::vec3 & axis);
    void translate(const glm::vec3 &v);
    
    const std::vector<unsigned int>& dirtyLightsIndexes() const { return mDirtyLights; }
    void setLightDirty(unsigned int index) { mDirtyLights.push_back(index); mLightsNeedUpdate = true;}
    void cleanDirtyLights() { mDirtyLights.clear(); mLightsNeedUpdate = false; }
    
    std::vector<DrawElementsIndirectCommand> const & drawCommands() const { return mDrawCommands; }
    std::vector<DrawElementsIndirectCommand> const & drawCommands(std::string material) const { return mDrawCommandsPerMaterial.at(material); }
    std::map<std::string, std::vector<DrawElementsIndirectCommand>> const &materialDrawCommands() const { return mDrawCommandsPerMaterial; }
    const std::vector<std::vector<DrawElementsIndirectCommand>>& materialDrawCommands2() const { return mDrawCommandsPerMaterial2; }
    const std::vector<unsigned int>& drawCommandsPerMaterialCount() const { return mDrawCommandCountPerMaterial; }
    
    void setDrawCommands(std::vector<DrawElementsIndirectCommand> drawCommands);
    void setDrawCommandsPerMaterial(std::map<std::string, std::vector<DrawElementsIndirectCommand>> drawCommands);
    void setDrawCommandsPerMaterial(const std::vector<std::vector<DrawElementsIndirectCommand>>& drawCommands);
    void setDrawCommandCountPerMaterial(const std::vector<unsigned int>& commands){ mDrawCommandCountPerMaterial = commands; }
    
    std::pair<unsigned int, unsigned int> drawMeshOffset(std::string modelName) const { return mDrawMeshOffset.at(modelName); }
    MeshOffsets drawMeshOffset(int index);

    void setDrawMeshOffset(std::string modelName, std::pair< unsigned int, unsigned int> offset) { mDrawMeshOffset[modelName] = offset; }
    void setDrawMeshOffset(int index, MeshOffsets offset);
    
    int drawInstanceIndex(std::string modelName) const { return mDrawInstanceIndex.at(modelName);  }
    unsigned int drawModelOffset(const std::array<int, 3>& index) const;
    void setDrawInstanceIndex(std::string modelName, int index){ mDrawInstanceIndex[modelName] = index; }
    bool setDrawModelOffset(const std::array<int, 3>& index, unsigned int modelOffset);
    
    void setRenderer(GLRenderer* renderer) { mRenderer = renderer; }
    
private:
    int mIndex = -1;
    std::string mName;
    std::vector< Mesh* > mMeshes;
    std::vector< Material* > mMaterials;
    std::vector< Model3D* > mModels;
    std::vector< Light* > mLights;
    std::vector< Camera* > mCameras;

    std::map <std::string, std::vector<Model3D* > > mMeshInstances;
    std::map <std::string, std::vector<Model3D* > > mMaterialInstances;
    std::map <std::string, innerMap > mMeshInstancesPerMaterial;
    std::map <int, innerMap2> mModelsPerMaterialPerMesh;
    std::map <std::string, innerMap > mMeshInstances2;
    std::map <int, innerMap2> mModelsPerMeshPerMaterial;
        
    std::vector<float> mPerFrameData;
    std::vector<float> mSceneData;
    std::vector<DrawElementsIndirectCommand> mDrawCommands;
    std::map<std::string, std::vector<DrawElementsIndirectCommand>> mDrawCommandsPerMaterial;
    std::vector<std::vector<DrawElementsIndirectCommand>> mDrawCommandsPerMaterial2;
    std::vector<unsigned int> mDrawCommandCountPerMaterial;
    std::map<std::string, std::pair<unsigned int, unsigned int>> mDrawMeshOffset;
    std::vector<MeshOffsets> mDrawMeshOffset2;
    std::map<std::string, int> mDrawInstanceIndex;   
    std::map<std::array<int, 3>, int> mDrawModelOffset;

    Camera* mCurrentCamera = nullptr;        

    bool mMeshCountChanged = false;
    bool mModelCountChanged = false;
    bool mViewCameraNeedUpdate = false;
    bool mProjCameraNeedUpdate = false;
    bool mLightsNeedUpdate = false;
    bool mMeshesNeedUpdate = false;
    bool mModelsNeedUpdate = false;
    bool mTexturesNeedUpdate = false;
    
    std::vector<unsigned int> mDirtyLights;
    std::vector<Mesh *> mDirtyMeshes;
    std::set<int> mDirtyMeshes2;
    std::set<int> mMaterialsAffectedByDirtyModels;
    std::vector<Model3D *> mDirtyModels;
    std::set<std::array<int, 3>> mDirtyModels2;
    std::vector<unsigned int> mDirtyTextures;
    
    GLRenderer* mRenderer = nullptr;
};

#endif // SCENE3D_H

#include "scene3d.h"
#include "glrenderer.h"
#include "mesh.h"
#include "material.h"
#include "model3d.h"

#include <algorithm>
#include <iterator>


Scene3D::Scene3D(std::string name, std::vector< Camera* > cameras, std::vector< Model3D* > models, std::vector< Light* > lights)
:
    mName(name),
    mCameras(cameras),
    mLights(lights)
{
    if(cameras.size())
        mCurrentCamera = mCameras.front();
    
    for(auto model : models)
        insertModel(model);
}

Scene3D::Scene3D(const Scene3D& other)
{
    mName = other.name();
    mCameras = other.cameras();
    mLights = other.lights();

    if (mCameras.size())
        mCurrentCamera = mCameras.front();

    for (auto model : other.models())
        insertModel(model);
}


Scene3D::~Scene3D()
{

}

//*** Public Methods ***//

bool Scene3D::insertMesh(Mesh* mesh)
{
    if(mesh->sceneIndex(this) < 0)
    {
        mesh->addScene(this, mMeshes.size());
        mMeshes.push_back(mesh);
                
        auto max = std::numeric_limits<unsigned int>::max();
        MeshOffsets offsets{ max, max, max, max, {}};        
        mDrawMeshOffset2.push_back(offsets);
        
        mMeshCountChanged = true;
        
        return true;        
    }
    else
        return false;
}

bool Scene3D::insertMaterial(Material* material)
{
    if(material->sceneIndex(this) < 0)
    {   
        material->addScene(this, mMaterials.size());
        mMaterials.push_back(material);        
        
        return true;
    }
    else
        return false;
}


bool Scene3D::insertModel(Model3D* model)
{
    if(model->sceneIndex(this)[2] < 0)
    {
        insertMesh(model->mesh());
                                                
        auto material = model->materials()[0];        
        if(insertMaterial(material))
            mDrawCommandCountPerMaterial.push_back(0);
        
        auto meshIndex = model->mesh()->sceneIndex(this);
        auto materialIndex = material->sceneIndex(this);
                        
        insertGeometryInstance(model->mesh()->name(), model);        
        insertMaterialInstance(material->name(), model);
                
        mModels.push_back(model);
        model->addScene(this, static_cast<int>(mModelsPerMeshPerMaterial[meshIndex][materialIndex].size()));
        mModelsPerMeshPerMaterial[meshIndex][materialIndex].push_back(model);
        mModelsPerMaterialPerMesh[materialIndex][meshIndex].push_back(model);
        
        mMaterialsAffectedByDirtyModels.insert(materialIndex);
        
        if(mDrawMeshOffset2[meshIndex].commandPerMaterial.find(materialIndex) == end(mDrawMeshOffset2[meshIndex].commandPerMaterial))
            mDrawMeshOffset2[meshIndex].commandPerMaterial[materialIndex] = std::numeric_limits<unsigned int>::max();
        
        setMeshesDirtyAfter(model->mesh());
        setModelDirty(model);
        
        auto initMatIdx = materialIndex + 1;
        
        for(auto meshIdx = meshIndex; meshIdx < static_cast<int>(mMeshes.size()); ++meshIdx)
        {
            for(auto matIter = mModelsPerMeshPerMaterial[meshIdx].find(initMatIdx); matIter != end(mModelsPerMeshPerMaterial[meshIdx]); ++matIter)
            {
                for (auto model : matIter->second)
                {
                    setModelDirty(model);

                    if (drawModelOffset(model->sceneIndex(this)) != std::numeric_limits<unsigned int>::max())
                        mDrawModelOffset[model->sceneIndex(this)]++;
                }
            }
            
            initMatIdx = 0;
        }
                
        mModelCountChanged = true;

        //mRenderer->setSceneDirty(mIndex);
        
        return true;
    }
    else
        return false;
}

bool Scene3D::insertCamera(Camera* camera, bool current)
{    
    if(findCamera(camera->name()) == nullptr)
    {
        mCameras.push_back(camera);
        if(current) mCurrentCamera = camera;
        return true;
    }
    else
        return false;
}

void Scene3D::insertGeometryInstance(std::string mesh, Model3D* model)
{
    mMeshInstances[mesh].push_back(model);
}

void Scene3D::insertMaterialInstance(std::string material, Model3D* model)
{
    mMaterialInstances[material].push_back(model);
    mMeshInstancesPerMaterial[material][model->mesh()->name()].push_back(model);
    mMeshInstances2[model->mesh()->name()][material].push_back(model);
}

void Scene3D::clearScene()
{
    invalidateScene();
    
    mDirtyLights.clear();
    
    for(const auto& mesh : mMeshes)
        mesh->removeScene(this);
    mMeshes.clear();
    mMeshInstances.clear();
    mMeshInstancesPerMaterial.clear();
    mMeshInstances2.clear();
    mDirtyMeshes2.clear();

    for(const auto& model : mModels)
        model->removeScene(this);
    mModels.clear();
    mModelsPerMaterialPerMesh.clear();
    mModelsPerMeshPerMaterial.clear();
    mDirtyModels2.clear();
    
    for(const auto& mat : mMaterials)
        mat->removeScene(this);    
    mMaterials.clear();    
    mMaterialInstances.clear();
    mMaterialsAffectedByDirtyModels.clear();

    mDrawMeshOffset.clear();
    mDrawMeshOffset2.clear();
    mDrawModelOffset.clear();
    mDrawCommands.clear();
    mDrawCommandsPerMaterial.clear();
    mDrawCommandsPerMaterial2.clear();
    mDrawCommandCountPerMaterial.clear();

    mMeshCountChanged = true;
    mModelCountChanged = true;
}

void Scene3D::clearCameras()
{
    mCameras.clear();
}

bool Scene3D::removeCamera(Camera * camera)
{
    auto search = std::find_if(begin(mCameras), end(mCameras), [&camera](Camera* currentCamera) { return (currentCamera == camera) ? true : false; });

    if (search != end(mCameras))
    {
        mCameras.erase(search);
        return true;
    }
    
    return false;
}

Mesh* Scene3D::findMesh(std::string meshName) const
{
    for(auto mesh: mMeshes)
        if(mesh->name() == meshName)
            return mesh;
    return nullptr;
}

Material* Scene3D::findMaterial(std::string materialName) const
{
    for(auto material: mMaterials)
        if(material->name() == materialName)
            return material;
    return nullptr;
}


Model3D* Scene3D::findModel3D(std::string modelName) const
{
    for(auto model: mModels)
        if(model->name() == modelName)
            return model;
   return nullptr;    
}

Light* Scene3D::findLight(std::string lightName) const
{
    for(auto light: mLights)
        if(light->name() == lightName)
            return light;
    return nullptr;
}

Camera* Scene3D::findCamera(std::string cameraName) const
{
    for(auto camera: mCameras)
        if(camera->name() == cameraName)
            return camera;
    return nullptr;
}

Model3D * Scene3D::previousModel(const std::array<int, 3>& index) const
{
    if(index[2] > 0)
        return mModelsPerMeshPerMaterial.at(index[0]).at(index[1])[index[2] - 1];
    else        
    {
        auto prevIter = std::prev(mModelsPerMeshPerMaterial.at(index[0]).find(index[1]), 1);
        
        if(prevIter != mModelsPerMeshPerMaterial.at(index[0]).begin())
            return (*prevIter).second[(*prevIter).second.size() - 1];
        else
        {
            auto prevIter0 = std::prev(mModelsPerMeshPerMaterial.find(index[0]));
            
            if(prevIter0 != mModelsPerMeshPerMaterial.begin())
                return (*prevIter0).second.cbegin()->second[(*prevIter0).second.cbegin()->second.size() - 1];
            else
                return nullptr;
        }
    }
}


/*void Scene3D::setModelDirty(unsigned int index)
{
    mDirtyModels.push_back(index); 
    mModelsNeedUpdate = true;
}*/

void Scene3D::setMeshDirty(Mesh* mesh)
{
    if(mesh != nullptr)
    {
        auto sceneIndex = mesh->sceneIndex(this);
        
        if (sceneIndex > -1)
        {
            mDirtyMeshes2.insert(sceneIndex);
            mDirtyMeshes.push_back(mesh);
            mMeshesNeedUpdate = true;
            
            mRenderer->setSceneDirty(mIndex);            
        }
    }    
}

void Scene3D::setMeshesDirtyAfter(Mesh* mesh)
{
    setMeshDirty(mesh);
    auto search = std::find(begin(mMeshes), end(mMeshes), mesh);
        
    if(search != end(mMeshes))
    {
        auto firstIndex = (*search)->sceneIndex(this);
        
        invalidateSceneFromMesh(firstIndex);
        
        auto invalidValue = std::numeric_limits<unsigned int>::max();
                    
        for(auto idx = firstIndex; idx < static_cast<int>(mMeshes.size()); ++idx)
        {
            mDirtyMeshes2.insert(idx);
            mMeshes[idx]->setBufferDirty(GLBuffer::Vertex);
            mMeshes[idx]->setBufferDirty(GLBuffer::Normal);
            mMeshes[idx]->setBufferDirty(GLBuffer::UV);
            mMeshes[idx]->setBufferDirty(GLBuffer::Color);
            mMeshes[idx]->setBufferDirty(GLBuffer::Index);
            
            mDrawMeshOffset2[idx].attrib = invalidValue;
            mDrawMeshOffset2[idx].index = invalidValue;
            mDrawMeshOffset2[idx].instance = invalidValue;
        }

        mMeshesNeedUpdate = true;        
    }
}


void Scene3D::setMeshDirty(std::string meshName)
{
    setMeshDirty(findMesh(meshName));
}

const std::vector<Model3D *> Scene3D::dirtyModels2() const
{
    std::vector<Model3D *> temp; 
    for(auto idx : mDirtyModels2)
        temp.push_back(mModelsPerMeshPerMaterial.at(idx[0]).at(idx[1])[idx[2]]); 
    
    return temp;
}


void Scene3D::setModelDirty(Model3D* model)
{
    if(model != nullptr)
    {
        auto sceneIndex = model->sceneIndex(this);
        
        if (sceneIndex > std::array<int, 3>{-1, -1, -1})
        {
            mDirtyModels2.insert(sceneIndex); 
            mDirtyModels.push_back(model);
            mDrawModelOffset[sceneIndex] = std::numeric_limits<unsigned int>::max();
            mModelsNeedUpdate = true;
            
            mRenderer->setSceneDirty(mIndex);            
        }
    }       
}

void Scene3D::setModelDirtyFrom(Model3D* model)
{
    //setModelDirty(model);
    auto search = std::find(begin(mModels), end(mModels), model);
        
    if(search != end(mModels))
    {
        
//         int firstInvalid = (*search)->sceneIndex(this);
// //         mRenderer->invalidateVertexDataFrom(mDrawMeshOffset2[firstInvalid].attrib);
// //         mRenderer->invalidateIndexDataFrom(mDrawMeshOffset2[firstInvalid].index);
//         mRenderer->invalidateInstanceDataFrom(mDrawMeshOffset2[firstInvalid].instance);
// 
//         
//         for(auto iter = search; iter != end(mModels); ++iter)
//             mDirtyModels2.insert((*iter)->sceneIndex(this));
//         
//         for(auto iter = mDirtyModels2.find(firstInvalid); iter != end(mDirtyModels2); ++iter)
//         {
// //             mModels[*iter]->setBufferDirty(GLBuffer::Vertex);
// //             mModels[*iter]->setBufferDirty(GLBuffer::Normal);
// //             mModels[*iter]->setBufferDirty(GLBuffer::UV);
// //             mModels[*iter]->setBufferDirty(GLBuffer::Color);
// //             mModels[*iter]->setBufferDirty(GLBuffer::Index);
//             
//             mDrawModelOffset[*iter] = std::numeric_limits<unsigned int>::max();
//         }

        mModelsNeedUpdate = true;
        mRenderer->setSceneDirty(mIndex);
    }
    
}


void Scene3D::setModelDirty(std::string modelName)
{
    auto modelSearch = findModel3D(modelName);
    
    if(modelSearch != nullptr)
    {
        if(std::find(begin(mDirtyModels), end(mDirtyModels), modelSearch) == end(mDirtyModels))
        {
            mDirtyModels.push_back(modelSearch);
            mModelsNeedUpdate = true;
        }
    }
}

void Scene3D::setFullUpdate()
{
    if(mMeshes.size())
        setMeshesDirtyAfter(mMeshes[0]);
    
    for(auto model: mModels)
        setModelDirty(model);
}

void Scene3D::invalidateSceneFromMesh(unsigned int index)
{
    if(mRenderer != nullptr && mMeshes.size() > index)
    {
        mRenderer->invalidateVertexDataFrom(mDrawMeshOffset2[index].attrib);
        mRenderer->invalidateIndexDataFrom(mDrawMeshOffset2[index].index);
        mRenderer->invalidateInstanceDataFrom(mDrawMeshOffset2[index].instance);
        
        mRenderer->setSceneDirty(mIndex);
    }    
}

void Scene3D::setOrientation(const glm::quat & orientation)
{
    for (const auto& model : mModels)
        model->setOrientation(orientation);
}

void Scene3D::setPosition(const glm::vec3 & position)
{
    for (const auto& model : mModels)
        model->setPosition(position);
}

void Scene3D::rotate(float angle, const glm::vec3& axis)
{
    for(const auto& model: mModels)
        model->rotate(angle, axis);
}

void Scene3D::translate(const glm::vec3& v)
{
    for(const auto& model: mModels)
        model->translate(v);
}

void Scene3D::setDrawCommands(std::vector< DrawElementsIndirectCommand > drawCommands)
{
    mDrawCommands = std::move(drawCommands);
}

void Scene3D::setDrawCommandsPerMaterial(std::map< std::string, std::vector<DrawElementsIndirectCommand> > drawCommands)
{
    mDrawCommandsPerMaterial = std::move(drawCommands);
}

void Scene3D::setDrawCommandsPerMaterial(const std::vector<std::vector<DrawElementsIndirectCommand> >& drawCommands)
{
    mDrawCommandsPerMaterial2 = drawCommands;
}

MeshOffsets Scene3D::drawMeshOffset(int index)
{
//     if(index >= static_cast<int>(mDrawMeshOffset2.size()) && index < static_cast<int>(mMeshes.size()))
//     {
//         auto max = std::numeric_limits<unsigned int>::max();
//         MeshOffsets offsets{ max, max, max, max, {{0, max}} };
//         
//         for(int mat = 1; mat < mMaterials.size(); ++ mat)
//             offsets.commandPerMaterial[mat] = max;
//         
//         mDrawMeshOffset2.resize(mMeshes.size(), offsets);        
//     }
    
    return mDrawMeshOffset2[index];    
}

void Scene3D::setDrawMeshOffset(int index, MeshOffsets offset)
{
//     if(index >= static_cast<int>(mDrawMeshOffset2.size()) && index < static_cast<int>(mMeshes.size()))
//     {
//         auto max = std::numeric_limits<unsigned int>::max();
//         MeshOffsets offsets{ max, max, max, max, {{0, max}} };
//         
//         for(int mat = 1; mat < mMaterials.size(); ++ mat)
//             offsets.commandPerMaterial[mat] = max;
//         
//         mDrawMeshOffset2.resize(mMeshes.size(), offsets);        
//     }
    
    mDrawMeshOffset2[index] = offset;   
}



unsigned int Scene3D::drawModelOffset(const std::array<int, 3>& index) const
{
    auto search = mDrawModelOffset.find(index);
    
    return (search != end(mDrawModelOffset)) ? search->second : std::numeric_limits<unsigned int>::max();
}

bool Scene3D::setDrawModelOffset(const std::array<int, 3>& index, unsigned int modelOffset)
{
    if(index[0] < static_cast<int>(mMeshes.size()) && index[1] < static_cast<int>(mMaterials.size()) && index[2] < static_cast<int>(mModelsPerMeshPerMaterial[index[0]][index[1]].size()))
    {
        mDrawModelOffset[index] = modelOffset;
        return true;
    }
    else
        return false;    
}

#include "fbxexporter.h"
#include "resourcemanager.h"
#include "scene3d.h"
#include "logger.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

FBXExporter::FBXExporter()
{
    mManager = FbxManager::Create();
    
    if(mManager == nullptr)
    {
        LOG("Error: Unable to create FBX Manager!");
        exit(1);
    }
    else
        LOG ("Autodesk FBX SDK version", mManager->GetVersion());    

    auto ios = FbxIOSettings::Create(mManager, IOSROOT);
    mManager->SetIOSettings(ios);

    // ... Configure the FbxIOSettings object here ...    
}

FBXExporter::~FBXExporter()
{    
    mManager->Destroy();    
}


// *** Public Methods *** //

void FBXExporter::exportSceneToFile(std::string filepath, Scene3D* scene)
{
    mFbxScene = FbxScene::Create(mManager, scene->name().c_str());
    
    if(mFbxScene == nullptr)
    {
        LOG("Error: Unable to create FBX scene!");
        exit(1);
    }
    
    populateFBXScene(mFbxScene, scene);    

    auto mExporter = FbxExporter::Create(mManager, "");
    mExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

    auto exportStatus = mExporter->Initialize(filepath.c_str(), -1, mManager->GetIOSettings());
    auto status = mExporter->Export(mFbxScene); 
    mExporter->Destroy();
}

void FBXExporter::exportSegmentedModelToFile(std::string filepath, Model3D* model, const std::map<std::string, std::vector<uint32_t> >& segmentation)
{    
    mFbxScene = FbxScene::Create(mManager, model->name().c_str());
    
    if(mFbxScene == nullptr)
    {
        LOG("Error: Unable to create FBX scene!");
        exit(1);
    }
    
    populateFBXScene(mFbxScene, model, segmentation);    

    auto mExporter = FbxExporter::Create(mManager, "");
    mExporter->SetFileExportVersion(FBX_2014_00_COMPATIBLE);

    // Initialize the exporter.
    auto exportStatus = mExporter->Initialize(filepath.c_str(), -1, mManager->GetIOSettings());    
    auto status = mExporter->Export(mFbxScene); 
    
    mExporter->Destroy();
}


// *** Private Methods *** //

void FBXExporter::populateFBXScene(FbxScene* fbxScene, Scene3D* scene)
{
    auto rootNode = fbxScene->GetRootNode();
     
    for(const auto& model : scene->models())
    {
        auto mesh = model->mesh();

        auto fbxMesh = FbxMesh::Create(fbxScene, mesh->name().c_str());
        
        auto normalElement= fbxMesh->CreateElementNormal();
        normalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        normalElement->SetReferenceMode(FbxGeometryElement::eDirect);

        auto vColorElement= fbxMesh->CreateElementVertexColor();
        vColorElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        vColorElement->SetReferenceMode(FbxGeometryElement::eDirect);
        
        auto uvElement = fbxMesh->CreateElementUV("UVSet1");
        uvElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        uvElement->SetReferenceMode(FbxGeometryElement::eDirect);
        
//         FbxGeometryElementMaterial* materialElement = fbxMesh->CreateElementMaterial();
//         materialElement->SetMappingMode(FbxGeometryElement::eAllSame);
        
        fbxMesh->InitControlPoints(mesh->elementCount());
        
        auto controlPoints = fbxMesh->GetControlPoints();
        auto vertexBuffer = reinterpret_cast<const float *>(mesh->vertexBuffer().data());
        auto normalBuffer = reinterpret_cast<const float *>(mesh->normalBuffer().data());
        auto uvBuffer = reinterpret_cast<const float *>(mesh->uvBuffer().data());
        auto colorBuffer = reinterpret_cast<const float *>(mesh->colorBuffer().data());
        auto indexBuffer = reinterpret_cast<const uint32_t *>(mesh->indexBuffer().data());
                
        for(auto i = 0; i < mesh->elementCount(); ++i)
        {
            controlPoints[i] = {vertexBuffer[3 * i], vertexBuffer[3 * i + 1], vertexBuffer[3 * i + 2], 1.0};
            normalElement->GetDirectArray().Add({normalBuffer[3 * i], normalBuffer[3 * i + 1], normalBuffer[3 * i + 2], 1.0});
            uvElement->GetDirectArray().Add({uvBuffer[2 * i], uvBuffer[2 * i + 1]});
            vColorElement->GetDirectArray().Add({colorBuffer[4 * i], colorBuffer[4 * i + 1], colorBuffer[4 * i + 2], colorBuffer[4 * i + 3]});
        }
        
        for(auto i = 0; i < mesh->elementCount(); i+=3)
        {
            fbxMesh->BeginPolygon();
            fbxMesh->AddPolygon(indexBuffer[i]);
            fbxMesh->AddPolygon(indexBuffer[i + 1]);
            fbxMesh->AddPolygon(indexBuffer[i + 2]);
            fbxMesh->EndPolygon();
        }
        
        auto node = FbxNode::Create(fbxScene, model->name().c_str());
        node->SetNodeAttribute(fbxMesh);
        
        auto eulerAngles = glm::eulerAngles(model->orientation());
        node->LclRotation.Set({eulerAngles.x, eulerAngles.y, eulerAngles.z});

        auto position = model->position();
        node->LclTranslation.Set({position.x, position.y, position.z});
        
        auto scale = model->scale();
        node->LclScaling.Set({scale.x, scale.y, scale.z});
        
        rootNode->AddChild(node);
    }     
}

void FBXExporter::populateFBXScene(fbxsdk::FbxScene* fbxScene, Model3D* model, const std::map<std::string, std::vector<uint32_t> >& segmentation)
{
    auto rootNode = fbxScene->GetRootNode();
     
    auto mesh = model->mesh();
    auto vertexBuffer = reinterpret_cast<const float *>(mesh->vertexBuffer().data());
    auto normalBuffer = reinterpret_cast<const float *>(mesh->normalBuffer().data());
    auto uvBuffer = reinterpret_cast<const float *>(mesh->uvBuffer().data());
    auto colorBuffer = reinterpret_cast<const float *>(mesh->colorBuffer().data());

    for(const auto& pair : segmentation)
    {
        const auto& indices = pair.second;

        auto fbxMesh = FbxMesh::Create(fbxScene, pair.first.c_str());

        auto normalElement= fbxMesh->CreateElementNormal();
        normalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        normalElement->SetReferenceMode(FbxGeometryElement::eDirect);

        auto vColorElement= fbxMesh->CreateElementVertexColor();
        vColorElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        vColorElement->SetReferenceMode(FbxGeometryElement::eDirect);

        auto uvElement = fbxMesh->CreateElementUV("UVSet1");
        uvElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
        uvElement->SetReferenceMode(FbxGeometryElement::eDirect);

        //         FbxGeometryElementMaterial* materialElement = fbxMesh->CreateElementMaterial();
        //         materialElement->SetMappingMode(FbxGeometryElement::eAllSame);

        fbxMesh->InitControlPoints(indices.size());
        auto controlPoints = fbxMesh->GetControlPoints();

        for(auto i = 0; i < indices.size(); ++i)
        {
            auto idx = indices[i];
            controlPoints[i] = {vertexBuffer[3 * idx], vertexBuffer[3 * idx + 1], vertexBuffer[3 * idx + 2], 1.0};
            normalElement->GetDirectArray().Add({normalBuffer[3 * idx], normalBuffer[3 * idx + 1], normalBuffer[3 * idx + 2], 1.0});
            uvElement->GetDirectArray().Add({uvBuffer[2 * idx], uvBuffer[2 * idx + 1]});
            vColorElement->GetDirectArray().Add({colorBuffer[4 * idx], colorBuffer[4 * idx + 1], colorBuffer[4 * idx + 2], colorBuffer[4 * idx + 3]});
        }

        for(auto i = 0; i < indices.size(); i += 3)
        {
            fbxMesh->BeginPolygon();
            fbxMesh->AddPolygon(i);
            fbxMesh->AddPolygon(i + 1);
            fbxMesh->AddPolygon(i + 2);
            fbxMesh->EndPolygon();
        }
        
//         Index Count > vertex count
//         std::set<uint32_t> unique;
//         std::vector<uint32_t> newIndices;
// 
//         for(const auto& index : indices)
//         {
//             auto result = unique.insert(index);            
//             if(result.second)
//                 newIndices.push_back(static_cast<uint32_t>(unique.size()) - 1);
//             else
//                 newIndices.push_back(std::distance(begin(unique), result.first) - 1);
//         }
//         
//         
//         fbxMesh->InitControlPoints(unique.size());
//         auto controlPoints = fbxMesh->GetControlPoints();
// 
//         int i = 0;
//         for(const auto& idx: unique)
//         {
//             controlPoints[i] = {vertexBuffer[3 * idx], vertexBuffer[3 * idx + 1], vertexBuffer[3 * idx + 2], 1.0};
//             normalElement->GetDirectArray().Add({normalBuffer[3 * idx], normalBuffer[3 * idx + 1], normalBuffer[3 * idx + 2], 1.0});
//             uvElement->GetDirectArray().Add({uvBuffer[2 * idx], uvBuffer[2 * idx + 1]});
//             vColorElement->GetDirectArray().Add({colorBuffer[4 * idx], colorBuffer[4 * idx + 1], colorBuffer[4 * idx + 2], colorBuffer[4 * idx + 3]});
//             ++i;
//         }
//         
//         for(auto i = 0; i < newIndices.size(); i += 3)
//         {
//             fbxMesh->BeginPolygon();
//             fbxMesh->AddPolygon(newIndices[i]);
//             fbxMesh->AddPolygon(newIndices[i + 1]);
//             fbxMesh->AddPolygon(newIndices[i + 2]);
//             fbxMesh->EndPolygon();
//         }           
        
        auto node = FbxNode::Create(fbxScene, pair.first.c_str());
        node->SetNodeAttribute(fbxMesh);

        auto eulerAngles = glm::eulerAngles(model->orientation());
        node->LclRotation.Set({eulerAngles.x, eulerAngles.y, eulerAngles.z});

        auto position = model->position();
        node->LclTranslation.Set({position.x, position.y, position.z});

        auto scale = model->scale();
        node->LclScaling.Set({scale.x, scale.y, scale.z});

        rootNode->AddChild(node);
    } 
}

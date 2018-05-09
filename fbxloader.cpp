#include "fbxloader.h"
#include "logger.hpp"

#include "resourcemanager.h"
#include "scene3d.h"
#include "model3d.h"
#include "mesh.h"


namespace
{
    const float ANGLE_TO_RADIAN = 3.1415926f / 180.f;
    const GLfloat BLACK_COLOR[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat GREEN_COLOR[] = {0.0f, 1.0f, 0.0f, 1.0f};
    const GLfloat WHITE_COLOR[] = {1.0f, 1.0f, 1.0f, 1.0f};
    const GLfloat WIREFRAME_COLOR[] = {0.5f, 0.5f, 0.5f, 1.0f};

    const int TRIANGLE_VERTEX_COUNT = 3;

    // Four floats for every position.
    const int VERTEX_STRIDE = 3;
    // Four floats for every color.
    const int COLOR_STRIDE = 4;
    // Three floats for every normal.
    const int NORMAL_STRIDE = 3;
    // Two floats for every UV.
    const int UV_STRIDE = 2;

    const GLfloat DEFAULT_LIGHT_POSITION[] = {0.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat DEFAULT_DIRECTION_LIGHT_POSITION[] = {0.0f, 0.0f, 1.0f, 0.0f};
    const GLfloat DEFAULT_SPOT_LIGHT_DIRECTION[] = {0.0f, 0.0f, -1.0f};
    const GLfloat DEFAULT_LIGHT_COLOR[] = {1.0f, 1.0f, 1.0f, 1.0f};
    const GLfloat DEFAULT_LIGHT_SPOT_CUTOFF = 180.0f;
}


FBXLoader::FBXLoader(ResourceManager *manager)
    :
    mResourceManager(manager)
{
    mManager = FbxManager::Create();

    FbxIOSettings *ioSettings = FbxIOSettings::Create(mManager, IOSROOT);
    mManager->SetIOSettings(ioSettings);

    FbxScene *scene = FbxScene::Create(mManager,"LoadedScene");
}


FBXLoader::FBXLoader(ResourceManager *manager, Scene3D *scene, std::string path)
    :
    mResourceManager(manager),
    mScene3D(scene)
{
    mManager = FbxManager::Create();

    FbxIOSettings *ioSettings = FbxIOSettings::Create(mManager, IOSROOT);
    mManager->SetIOSettings(ioSettings);
    
    loadScene(scene, path);
}

FBXLoader::~FBXLoader()
{

}

void FBXLoader::loadScene(Scene3D* scene, std::string path)
{
    int SDKMajor, SDKMinor, SDKRevision, fileMajor, fileMinor, fileRevision;
    mManager->GetFileFormatVersion(SDKMajor, SDKMinor, SDKRevision);

    FbxImporter *importer = FbxImporter::Create(mManager, "");
    bool importStatus = importer->Initialize(path.c_str(), -1, mManager->GetIOSettings());
    importer->GetFileVersion(fileMajor, fileMinor, fileRevision);

    LOG("FBXLoader::loadScene: FBX Version: ", SDKMajor,", ", SDKMinor, ", ", SDKRevision);
    LOG("FBXLoader::loadScene: File Version: ", fileMajor, ", ", fileMinor, ", ", fileRevision);

    if(!importStatus)
        LOG_ERR("FBXLoader::loadScene: FbxImporter::Initialize failed! \n. Error return: ", importer->GetStatus().GetErrorString(), "\n");
    else
    {
        mScene3D = scene;

        if(mFbxScene != nullptr)
            mFbxScene->Destroy();
        
        mFbxScene = FbxScene::Create(mManager,"LoadedScene");
        
        importer->Import(mFbxScene);
        
        FbxGeometryConverter geomConverter(mManager);
        geomConverter.Triangulate(mFbxScene, /*replace*/true);

        extractLights();
       
        FbxNode *rootNode = mFbxScene->GetRootNode();

        if(rootNode != nullptr)
        {
            int childCount = mFbxScene->GetRootNode()->GetChildCount();

            for(int child = 0; child < childCount; ++child)
                extractNode(mFbxScene->GetNode(child));
        }
    }

    importer->Destroy();
}


void FBXLoader::extractWordsFromNextLine(std::ifstream& file, std::vector< std::string, std::allocator< void > >& words, const std::string& delimiter)
{

}

void FBXLoader::extractWordsFromLine(std::ifstream& file, std::string& line, std::vector< std::string, std::allocator< void > >& words, const std::string& delimiter)
{

}

void FBXLoader::extractWords(std::vector< std::string, std::allocator< void > >& words, std::string& line, const std::string& delimiter)
{

}

FbxAMatrix FBXLoader::getGlobalPosition(FbxNode* node, const FbxTime& time, FbxPose* pose, FbxAMatrix* parentGlobalPosition)
{
    FbxAMatrix globalPosition;
    bool positionFound = false;

    if (pose)
    {
        int nodeIndex = pose->Find(node);

        if (nodeIndex > -1)
        {
            // The bind pose is always a global matrix.
            // If we have a rest pose, we need to check if it is
            // stored in global or local space.
            if (pose->IsBindPose() || !pose->IsLocalMatrix(nodeIndex))
            {
                globalPosition = getPoseMatrix(pose, nodeIndex);
            }
            else
            {
                // We have a local matrix, we need to convert it to
                // a global space matrix.
                FbxAMatrix currentParentGlobalPosition;

                if (parentGlobalPosition)
                {
                    currentParentGlobalPosition = *parentGlobalPosition;
                }
                else
                {
                    if (node->GetParent())
                    {
                        currentParentGlobalPosition = getGlobalPosition(node->GetParent(), time, pose);
                    }
                }

                FbxAMatrix localPosition = getPoseMatrix(pose, nodeIndex);
                globalPosition = currentParentGlobalPosition * localPosition;
            }

            positionFound = true;
        }
    }

    if (!positionFound)
    {
        // There is no pose entry for that node, get the current global position instead.

        // Ideally this would use parent global position and local position to compute the global position.
        // Unfortunately the equation 
        //    globalPosition = parentGlobalPosition * localPosition
        // does not hold when inheritance type is other than "Parent" (RSrs).
        // To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
        globalPosition = node->EvaluateGlobalTransform(time);
    }

    return globalPosition;
}


FbxAMatrix FBXLoader::getPoseMatrix(FbxPose* pose, int nodeIndex)
{
    FbxAMatrix poseMatrix;
    FbxMatrix matrix = pose->GetMatrix(nodeIndex);

    memcpy((double*)poseMatrix, (double*)matrix, sizeof(matrix.mData));

    return poseMatrix;
}


FbxAMatrix FBXLoader::getGeometryOffset(FbxNode* node)
{
    const FbxVector4 t = node->GetGeometricTranslation(FbxNode::eSourcePivot);
    const FbxVector4 r = node->GetGeometricRotation(FbxNode::eSourcePivot);
    const FbxVector4 s = node->GetGeometricScaling(FbxNode::eSourcePivot);

    return FbxAMatrix(t, r, s);
}


void FBXLoader::extractNode(FbxNode* node)
{
    FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();


    if (nodeAttribute)
    {
        if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
        {
            //ÉãÏñ»úµÄÄ¿±ê
        }
        else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            //ExtractSkeleton(node, parentMat, worldMat);
        }
        // NURBS and patch have been converted into triangluation meshes.
        else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
        {
            counter++;
            std::string modelName (node->GetName());
            
            if(!mResourceManager->containModel(modelName))
            {
                LOG("Creating model ", modelName);
                Model3D * model = mResourceManager->createModel(modelName);//new Model3D(modelName);  
                extractTransform(node, model);
                
                FbxMesh *fbxMesh = nodeAttribute->GetNode()->GetMesh();
                std::string meshName(nodeAttribute->GetNode()->GetName());
                
                if(!mResourceManager->containMesh(meshName))
                {
                    LOG("Creating mesh ", meshName);
                    //Mesh * mesh = new Mesh(meshName);
                    Mesh * mesh = mResourceManager->createMesh(meshName);
                    extractMesh(fbxMesh, mesh);
                    //mResourceManager->insertMesh(meshName, mesh);
                    model->setMesh(mesh);
                    
                    mesh->setVAO();                
                }
                else
                    model->setMesh(mResourceManager->mesh(meshName));
                
                const int materialCount = node->GetMaterialCount();            
                for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
                {
                    FbxSurfaceMaterial *fbxMaterial = node->GetMaterial(materialIndex);
                    
                    std::string matName(fbxMaterial->GetName());
                    
                    if(!mResourceManager->containMaterial(matName))
                    {                                       
                        Material *material = mResourceManager->createMaterial(matName, "ProjectiveTex");//new Material(matName);                
                        extractMaterial(fbxMaterial, material);
                        //material->setShader(mResourceManager->shaderProgram("ProjectiveTex"));
                        //mResourceManager->insertMaterial(matName, material);                    
                        model->insertMaterial(material);
                        
                    }
                    else
                        model->insertMaterial(mResourceManager->material(matName));
                }
                 
                mScene3D->insertModel(model);
            }
        }
        else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
        {
            FbxCamera *fbxCamera = nodeAttribute->GetNode()->GetCamera();
            std::string cameraName(nodeAttribute->GetNode()->GetName());
            LOG("Entro en camera");
            if(mScene3D->findCamera(cameraName) == nullptr)
            {
                Camera *camera = new Camera(cameraName);
                extractCamera(fbxCamera, camera);
                //LOG("Fbx Camera View ", glm::to_string(camera->viewMatrix()), " DC: ", mFbxScene->GetGlobalSettings().GetDefaultCamera().Buffer());
                mScene3D->insertCamera(camera);
                
                mScene3D->updateCamera(camera);                    
            }
            //ExtractCamera(node, worldMat);
        }
//         else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
//         {
//             FbxLight *fbxLight = nodeAttribute->GetNode()->GetLight();
//             //Light *light = new Light(nodeAttribute->GetNode()->GetName());
//             extractLight(fbxLight, );
//         }
        else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
        {
            //ExtractNull(worldMat);
        }
    }

    int childCount = node->GetChildCount();

    for (int child = 0; child < childCount; ++child)
        extractNode(node->GetChild(child));
}


void FBXLoader::extractTransform(FbxNode* node, Model3D* model)
{
    const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot).Buffer();
    const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot).Buffer();
    const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot).Buffer();
    
    glm::dmat4 geomTransform = glm::make_mat4(static_cast<double *>(FbxAMatrix(T, R, S)));
    
    //std::cerr << "Geometric Trans Matrix: " << glm::to_string(geomTransform) << std::endl;
    
    glm::dmat4 localTransform = glm::make_mat4(static_cast<double *>(node->EvaluateLocalTransform()));
    
    //std::cerr << "Local Trans Matrix: " << glm::to_string(localTransform) << std::endl;
    
    model->setModelMatrix(glm::mat4(localTransform * geomTransform));
}



void FBXLoader::extractMesh(FbxMesh* fbxMesh, Mesh* mesh)
{    
    if (!fbxMesh->GetNode())
        std::cerr << "Error" << std::endl;
    //return false;

    const int polygonCount = fbxMesh->GetPolygonCount();

    // Count the polygon count of each material
    FbxLayerElementArrayTemplate<int>* materialIndices = NULL;
    FbxGeometryElement::EMappingMode materialMappingMode = FbxGeometryElement::eNone;

    FbxArray<SubMesh*> subMeshes;
    
    if (fbxMesh->GetElementMaterial())
    {
        materialIndices = &fbxMesh->GetElementMaterial()->GetIndexArray();
        materialMappingMode = fbxMesh->GetElementMaterial()->GetMappingMode();

        if (materialIndices && materialMappingMode == FbxGeometryElement::eByPolygon)
        {
            FBX_ASSERT(materialIndices->GetCount() == polygonCount);

            if (materialIndices->GetCount() == polygonCount)
            {
                // Count the faces of each material
                for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
                {
                    const int materialIndex = materialIndices->GetAt(polygonIndex);
                    if (subMeshes.GetCount() < materialIndex + 1)
                        subMeshes.Resize(materialIndex + 1);

                    if (subMeshes[materialIndex] == NULL)
                        subMeshes[materialIndex] = new SubMesh;

                    subMeshes[materialIndex]->TriangleCount += 1;
                }

                // Make sure we have no "holes" (NULL) in the subMeshes table. This can happen
                // if, in the loop above, we resized the subMeshes by more than one slot.
                for (int i = 0; i < subMeshes.GetCount(); i++)
                    if (subMeshes[i] == NULL)
                        subMeshes[i] = new SubMesh;

                // Record the offset (how many vertex)
                const int materialCount = subMeshes.GetCount();
                int offset = 0;
                for (int index = 0; index < materialCount; ++index)
                {
                    subMeshes[index]->IndexOffset = offset;
                    offset += subMeshes[index]->TriangleCount * 3;
                    // This will be used as counter in the following procedures, reset to zero
                    subMeshes[index]->TriangleCount = 0;
                }

                FBX_ASSERT(offset == polygonCount * 3);
            }
        }
    }

    // All faces will use the same material.
    if (subMeshes.GetCount() == 0)
    {
        subMeshes.Resize(1);
        subMeshes[0] = new SubMesh();
    }

    // Congregate all the data of a mesh to be cached in VBOs.
    // If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
    mHasColor = fbxMesh->GetElementVertexColorCount() > 0;
    mHasNormal = fbxMesh->GetElementNormalCount() > 0;
    mHasUV = fbxMesh->GetElementUVCount() > 0;
    FbxGeometryElement::EMappingMode colorMappingMode = FbxGeometryElement::eNone;
    FbxGeometryElement::EMappingMode normalMappingMode = FbxGeometryElement::eNone;
    FbxGeometryElement::EMappingMode UVMappingMode = FbxGeometryElement::eNone;

    if(mHasColor)
    {
        colorMappingMode = fbxMesh->GetElementVertexColor(0)->GetMappingMode();
        
        if(colorMappingMode == FbxGeometryElement::eNone)
            mHasColor = false;
        
        if(mHasColor && colorMappingMode != FbxGeometryElement::eByControlPoint)
            mAllByControlPoint = false;
    }
    
    if (mHasNormal)
    {
        normalMappingMode = fbxMesh->GetElementNormal(0)->GetMappingMode();

        if (normalMappingMode == FbxGeometryElement::eNone)
            mHasNormal = false;

        if (mHasNormal && normalMappingMode != FbxGeometryElement::eByControlPoint)
            mAllByControlPoint = false;
    }
    
    if (mHasUV)
    {
        UVMappingMode = fbxMesh->GetElementUV(0)->GetMappingMode();
        if (UVMappingMode == FbxGeometryElement::eNone)
            mHasUV = false;

        if (mHasUV && UVMappingMode != FbxGeometryElement::eByControlPoint)
            mAllByControlPoint = false;
    }

    // Allocate the array memory, by control point or by polygon vertex.
    int polygonVertexCount = fbxMesh->GetControlPointsCount();

    if (!mAllByControlPoint)
        polygonVertexCount = polygonCount * TRIANGLE_VERTEX_COUNT;

    std::vector<GLfloat> vertices(polygonVertexCount * VERTEX_STRIDE);
    std::vector<GLuint> indices(polygonCount * TRIANGLE_VERTEX_COUNT);
    std::vector<GLfloat> colors;
    std::vector<GLfloat>normals;

    if(mHasColor)
        colors.resize(polygonVertexCount * COLOR_STRIDE);
    else
        colors.resize(polygonVertexCount * COLOR_STRIDE);
    
    if(mHasNormal)
        normals.resize(polygonVertexCount * NORMAL_STRIDE);
    
    std::vector<GLfloat> UVs;
    FbxStringList UVNames;
    fbxMesh->GetUVSetNames(UVNames);
    const char * UVName = NULL;

    if (mHasUV && UVNames.GetCount())
    {
        UVs.resize(polygonVertexCount * UV_STRIDE);
        UVName = UVNames[0];
        
        //if(UVNames.GetCount() > 1)
        //    UVName = UVNames[1];
        
        std::cerr << "UVNames: ";
        
        for(int i = 0; i < UVNames.GetCount(); i++)
            std::cerr << UVNames[0] << " ";
        std::cerr << std::endl;
    }

    // Populate the array with vertex attribute, if by control point.
    const FbxVector4 * controlPoints = fbxMesh->GetControlPoints();
    FbxVector4 currentVertex;
    FbxColor currentColor;
    FbxVector4 currentNormal;
    FbxVector2 currentUV;

    if (mAllByControlPoint)
    {
        const FbxGeometryElementNormal * normalElement = NULL;
        const FbxGeometryElementUV * UVElement = NULL;

        if (mHasNormal)
            normalElement = fbxMesh->GetElementNormal(0);

        if (mHasUV)
            UVElement = fbxMesh->GetElementUV(0);

        for (int index = 0; index < polygonVertexCount; ++index)
        {
            // Save the vertex position.
            currentVertex = controlPoints[index];
            vertices[index * VERTEX_STRIDE] = static_cast<float>(currentVertex[0]);
            vertices[index * VERTEX_STRIDE + 1] = static_cast<float>(currentVertex[1]);
            vertices[index * VERTEX_STRIDE + 2] = static_cast<float>(currentVertex[2]);

            // Save the normal.
            if (mHasNormal)
            {
                int normalIndex = index;
                if (normalElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                    normalIndex = normalElement->GetIndexArray().GetAt(index);

                currentNormal = normalElement->GetDirectArray().GetAt(normalIndex);
                normals[index * NORMAL_STRIDE] = static_cast<float>(currentNormal[0]);
                normals[index * NORMAL_STRIDE + 1] = static_cast<float>(currentNormal[1]);
                normals[index * NORMAL_STRIDE + 2] = static_cast<float>(currentNormal[2]);
            }

            // Save the UV.
            if (mHasUV)
            {
                int UVIndex = index;
                if (UVElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                    UVIndex = UVElement->GetIndexArray().GetAt(index);

                currentUV = UVElement->GetDirectArray().GetAt(UVIndex);
                UVs[index * UV_STRIDE] = static_cast<float>(currentUV[0]);
                UVs[index * UV_STRIDE + 1] = static_cast<float>(currentUV[1]);
            }
        }

    }

    int vertexCount = 0;
    
    auto elementVertexColor = fbxMesh->GetElementVertexColor();

    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
    {
        // The material for current face.
        int materialIndex = 0;

        if (materialIndices && materialMappingMode == FbxGeometryElement::eByPolygon)
            materialIndex = materialIndices->GetAt(polygonIndex);

        // Where should I save the vertex attribute index, according to the material
        const int indexOffset = subMeshes[materialIndex]->IndexOffset +
                                subMeshes[materialIndex]->TriangleCount * 3;

        for (int verticeIndex = 0; verticeIndex < TRIANGLE_VERTEX_COUNT; ++verticeIndex)
        {
            const int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, verticeIndex);

            //LOG("FBXLoader::ControlPointIndex: ", controlPointIndex);
            
            if (mAllByControlPoint)
                indices[indexOffset + verticeIndex] = static_cast<unsigned int>(controlPointIndex);
            // Populate the array with vertex attribute, if by polygon vertex.
            else
            {
                indices[indexOffset + verticeIndex] = static_cast<unsigned int>(vertexCount);

                currentVertex = controlPoints[controlPointIndex];
                vertices[vertexCount * VERTEX_STRIDE] = static_cast<float>(currentVertex[0]);
                vertices[vertexCount * VERTEX_STRIDE + 1] = static_cast<float>(currentVertex[1]);
                vertices[vertexCount * VERTEX_STRIDE + 2] = static_cast<float>(currentVertex[2]);

                if(mHasNormal)
                {
                    fbxMesh->GetPolygonVertexNormal(polygonIndex, verticeIndex, currentNormal);
                    normals[vertexCount * NORMAL_STRIDE] = static_cast<float>(currentNormal[0]);
                    normals[vertexCount * NORMAL_STRIDE + 1] = static_cast<float>(currentNormal[1]);
                    normals[vertexCount * NORMAL_STRIDE + 2] = static_cast<float>(currentNormal[2]);
                }

                if(mHasUV)
                {
                    bool unmappedUV;
                    fbxMesh->GetPolygonVertexUV(polygonIndex, verticeIndex, UVName, currentUV, unmappedUV);
                    //FbxVector2 otra = fbxMesh->GetLayer(0)->GetUVs()->GetDirectArray().GetAt(controlPointIndex);
                    UVs[vertexCount * UV_STRIDE] = static_cast<float>(currentUV[0]);
                    UVs[vertexCount * UV_STRIDE + 1] = static_cast<float>(currentUV[1]);
                    
                }
                
                if(mHasColor)
                {
                    int colorIndex = 0;
                    if(elementVertexColor->GetReferenceMode() == FbxGeometryElement::eDirect )
                        colorIndex = vertexCount;

                    //reference mode is index-to-direct, get normals by the index-to-direct
                    if(elementVertexColor->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
                        colorIndex = elementVertexColor->GetIndexArray().GetAt(vertexCount);
                    
                    currentColor = elementVertexColor->GetDirectArray().GetAt(colorIndex);
                    
                    colors[vertexCount * COLOR_STRIDE] = static_cast<float>(currentColor.mRed);
                    colors[vertexCount * COLOR_STRIDE + 1] = static_cast<float>(currentColor.mGreen);
                    colors[vertexCount * COLOR_STRIDE + 2] = static_cast<float>(currentColor.mBlue);
                    colors[vertexCount * COLOR_STRIDE + 3] = static_cast<float>(currentColor.mAlpha);
                }
                else // Temporal fix for no color
                {
                    colors[vertexCount * COLOR_STRIDE] = 0.5;
                    colors[vertexCount * COLOR_STRIDE + 1] = 0.5;
                    colors[vertexCount * COLOR_STRIDE + 2] = 0.5;
                    colors[vertexCount * COLOR_STRIDE + 3] = 1.0;                    
                }
                //std::cerr << "Vertex: " << currentVertex[0] << " " << currentVertex[1] << " " << currentVertex[2] << " " << std::endl;
                //std::cerr << "Normal: " << currentNormal[0] << " " << currentNormal[1] << " " << currentNormal[2] << " " << std::endl;
                //std::cerr << "UV: " << currentUV[0] << " " << currentUV[1] << " " << currentUV[2] << " " << std::endl;
                
            }
            ++vertexCount;
        }

        subMeshes[materialIndex]->TriangleCount += 1;
    }
    
    std::vector<GLuint> subMeshIndexes(subMeshes.Size());
    
    for(auto index = 0; index < subMeshes.Size(); index++)
    {
	LOG("Offset: ", subMeshes[index]->IndexOffset, ", triangleCount: ", subMeshes[index]->TriangleCount);        
	subMeshIndexes[index] = subMeshes[index]->TriangleCount * 3;
    }
    
//     glm::vec3 vert1(vertices[0], vertices[1], vertices[2]);
//     glm::vec2 uv1(UVs[0], UVs[1]);
//     glm::vec3 vert2(vertices[3], vertices[4], vertices[5]);
//     glm::vec2 uv2(UVs[2], UVs[3]);
//     glm::vec3 vert3(vertices[6], vertices[7], vertices[8]);
//     glm::vec2 uv3(UVs[4], UVs[5]);
//     
//     uv1 = uv1 * 2048;
//     uv2 = uv2 * 2048;
//     uv3 = uv3 * 2048;
//     
//     float realLength[3];
//     float textLength[3];
//     
//     realLength[0] = glm::length(vert2 - vert1);
//     realLength[1] = glm::length(vert3 - vert2);
//     realLength[2] = glm::length(vert1 - vert3);
//     
//     textLength[0] = glm::length(uv2 - uv1);
//     textLength[1] = glm::length(uv3 - uv2);
//     textLength[2] = glm::length(uv1 - uv3);
//     
//     float realPer = (realLength[0] + realLength[1] + realLength[2]) / 2.0;
//     float texPer = (textLength[0] + textLength[1] + textLength[2]) / 2.0;
//     
//     float realArea = sqrt(realPer * (realPer - realLength[0]) * (realPer - realLength[1]) * (realPer - realLength[2]));
//     float texArea = sqrt(texPer * (texPer - textLength[0]) * (texPer - textLength[1]) * (texPer - textLength[2]));
//     
//     float areaPerPixel = realArea/texArea;
    
    
    fbxMesh->ComputeBBox();
    auto bbMax = fbxMesh->BBoxMax.Get();
    auto bbMin = fbxMesh->BBoxMin.Get();
    
    mesh->setBoundingBox({static_cast<float>(bbMin[0]), static_cast<float>(bbMin[1]), static_cast<float>(bbMin[2])}, {static_cast<float>(bbMax[0]), static_cast<float>(bbMax[1]), static_cast<float>(bbMax[2])});    
    
    mesh->updateData(GLBuffer::Vertex, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    mesh->updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    mesh->updateData(GLBuffer::UV, 0, UVs.size() * sizeof(GLfloat), UVs.data());
    mesh->updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());
    mesh->updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());
    //mesh->updateData(GL::IndexBuffer, 0, subMeshIndexes.size() * sizeof(GLuint), subMeshIndexes.data());

     mesh->updateIndexData(indices);
     mesh->updateSubMeshData(subMeshIndexes);
}

void FBXLoader::extractCamera(FbxCamera* fbxCamera, Camera* camera)
{
    LOG("Camera ", camera->name());
    
    LOG("- Projection Matrix ",  glm::to_string(glm::make_mat4(static_cast<double *>(fbxCamera->ComputeProjectionMatrix(1280, 720)))));
    
    camera->setProjectionMatrix(glm::mat4(glm::make_mat4(static_cast<double *>(fbxCamera->ComputeProjectionMatrix(1280, 720)))));
    camera->setAspectWidth(fbxCamera->AspectWidth.Get());
    camera->setAspectHeight(fbxCamera->AspectHeight.Get());
    camera->setAspectRatio(static_cast<double>(1280)/static_cast<double>(720));
    camera->setFieldOfViewX(fbxCamera->FieldOfViewX.Get());
    camera->setFieldOfViewY(fbxCamera->FieldOfViewY.Get());
    camera->setFieldOfView(fbxCamera->FieldOfView.Get());
    camera->setFocalLength(fbxCamera->FocalLength.Get());
    camera->setFarPlane(fbxCamera->FarPlane.Get());
    camera->setNearPlane(fbxCamera->NearPlane.Get());
    
    auto aspectRatio = static_cast<double>(1280)/static_cast<double>(720);
    auto glmPerspective = glm::perspective(glm::radians(camera->fieldOfViewY()), aspectRatio, camera->nearPlane(), camera->farPlane());
    //auto glmPerspective = glm::perspectiveFov(glm::radians(camera->fieldOfView()), 1280.0, 720.0, camera->nearPlane(), camera->farPlane());
    //camera->setProjectionMatrix(glmPerspective);
    LOG("- Proyection Matrix GLM: ", glm::to_string(glmPerspective));

    glm::dvec3 up = glm::make_vec3(fbxCamera->UpVector.Get().Buffer());    
    glm::dvec3 position = glm::make_vec3(fbxCamera->Position.Get().Buffer());
    //glm::dvec3 center = glm::make_vec3(fbxCamera->InterestPosition.Get().Buffer());
    glm::dvec3 center(0.0);
      
    LOG("- Up ", glm::to_string(up));
    LOG("- Position ", glm::to_string(position));
    LOG("- Center ", glm::to_string(center));
    LOG("- Width ", fbxCamera->AspectWidth.Get(), ", Height ", fbxCamera->AspectHeight.Get(), ", Ratio ", fbxCamera->PixelAspectRatio.Get());
    
    glm::mat4 cameraView = glm::mat4(glm::lookAt(position, center, up));
    LOG("- View Matrix ",  glm::to_string(cameraView));
    
    glm::quat quat = glm::quat_cast(cameraView);
//     quat.x *= -1.0f;
//     quat.y *= -1.0f;
//     quat.z *= -1.0f;
    
    glm::mat4 mat = glm::translate(glm::mat4_cast(quat), glm::vec3(-position));
    
    LOG("- Other View Matrix: ", glm::to_string(mat));

    glm::vec3 alternatePos(-cameraView[3][0], -cameraView[3][1], -cameraView[3][2]);

    //camera->setViewMatrix(cameraView);
    camera->setPosition(alternatePos);
    //camera->setForward(glm::vec3(position - center));
    camera->setUp(glm::vec3(up));
    camera->setOrientation(quat);    
    //camera->lookAt(glm::vec3(position), glm::vec3(center), glm::vec3(up));
}

void FBXLoader::extractLights()
{
    // Setting the lights before drawing the whole scene
    
    
    
    LOG("Pose count: ", mFbxScene->GetPoseCount());
    
    const int lightCount = mFbxScene->GetSrcObjectCount<FbxLight>();
    for (int lightIndex = 0; lightIndex < lightCount; ++lightIndex)
    {
        FbxLight * fbxLight = mFbxScene->GetSrcObject<FbxLight>(lightIndex);
        FbxLight::EType fbxLightType = fbxLight->LightType.Get();
        FbxNode * node = fbxLight->GetNode();
        FbxTime time;
                
        if (node)
        {
            FbxAMatrix globalPosition = getGlobalPosition(node, time, mFbxScene->GetPose(0));
            FbxAMatrix geometryOffset = getGeometryOffset(node);
            FbxAMatrix globalOffPosition = globalPosition * geometryOffset;
            
            glm::dmat4 lightMat = glm::make_mat4(static_cast<double *>(globalOffPosition));
            LOG("Mat ", glm::to_string(lightMat));

            const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot).Buffer();
            const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot).Buffer();
            const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot).Buffer();

            glm::dmat4 geomTransform = glm::make_mat4(static_cast<double *>(FbxAMatrix(T, R, S)));
            glm::dmat4 localTransform = glm::make_mat4(static_cast<double *>(node->EvaluateLocalTransform()));

            LOG("Mat alt: ", glm::to_string(glm::mat4(localTransform * geomTransform)));
                      
            Light* light = new Light(fbxLight->GetName(), static_cast<Light::lightType>(fbxLightType), glm::mat4(localTransform * geomTransform));
          
            LOG("Light: ", light->name(), " type ", light->type());
                        
            if(light->name().size() < 1)
                light->setName("Annonymous" + std::to_string(lightIndex));
            //Model3D *lightModel = new Model3D(light->name());
            //lightModel->setMesh(mResourceManager->mesh("Lights"));
            //lightModel->setModelMatrix(light->matrix());
            //lightModel->insertMaterial(mResourceManager->material("Lights"));
            //lightModel->material(0)->setShader(mResourceManager->shaderProgram("BillBoarding"));
            
            //if(mScene3D->findMesh("Lights") == nullptr)
            //    mScene3D->insertMesh(lightModel->mesh());

            mScene3D->insertLight(light);            
            //mScene3D->insertModel(lightModel);
            //mScene3D->insertMaterial(lightModel->material(0));
            //mScene3D->insertGeometryInstance("Lights", lightModel);
            //mScene3D->insertMaterialInstance("Lights", lightModel); 
        }
    }
}


void FBXLoader::extractMaterial(const FbxSurfaceMaterial* fbxMaterial, Material *material)
{    
    material->setEmissive(getMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor));
    material->setAmbient(getMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor));
    material->setDiffuse(getMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor));
    material->setSpecular(getMaterialProperty(fbxMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor));

    FbxProperty shininessProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (shininessProperty.IsValid())
        material->setShininess(static_cast<GLfloat>(shininessProperty.Get<FbxDouble>()));
}

// Get specific property value and connected texture if any.
// Value = Property value * Factor property value (if no factor property, multiply by 1).
ColorChannel FBXLoader::getMaterialProperty(const FbxSurfaceMaterial* material, const char* propertyName, const char* factorPropertyName)
{
    ColorChannel channel;
    const FbxProperty property = material->FindProperty(propertyName);
    const FbxProperty factorProperty = material->FindProperty(factorPropertyName);
    
    if (property.IsValid() && factorProperty.IsValid())
    {
        FbxDouble3 color(0, 0, 0);
        color = property.Get<FbxDouble3>();
        double factor = factorProperty.Get<FbxDouble>();
        if (factor != 1)
        {
            color[0] *= factor;
            color[1] *= factor;
            color[2] *= factor;
        }
        channel.mColor = glm::vec4(glm::make_vec3(color.Buffer()), 1.0);
    }

    if (property.IsValid())
    {
        const int textureCount = property.GetSrcObjectCount<FbxFileTexture>();
        if (textureCount)
        {
            const FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>();
            
            if (texture)
                channel.mTexture = mResourceManager->loadTexture(texture->GetFileName());
        }
    }
    
    return channel;
}

Texture *FBXLoader::getMaterialPropertyTexture(const FbxSurfaceMaterial* material, const char* propertyName)
{
    const FbxProperty property = material->FindProperty(propertyName);
    
    if (property.IsValid())
    {
        const int textureCount = property.GetSrcObjectCount<FbxFileTexture>();
        if (textureCount)
        {
            const FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>();
            
            if (texture)
                return mResourceManager->loadTexture(texture->GetFileName());
        }
    }
    
    return nullptr;
}

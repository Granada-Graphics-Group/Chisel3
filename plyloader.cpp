#include "plyloader.h"
#include "resourcemanager.h"
#include "scene3d.h"

#include <sstream>

constexpr std::array<size_t, 9> PLYProperty::typeSizes;

PLYLoader::PLYLoader(ResourceManager* manager, Scene3D* scene, std::string filePath)
:
    mResourceManager(manager),
    mScene(scene)
{
    loadScene(filePath);
}


// *** Public Methods *** //

Scene3D * PLYLoader::loadScene(std::string filePath)
{
    std::ifstream fileStream;
    fileStream.open(filePath, std::ios::in);
    
    if(fileStream.good())
    {
        if(!readHeader(fileStream))
        {
            
        }
        
        readData(fileStream);
    }
    
    return mScene;
}


// *** Private Methods *** //

bool PLYLoader::readHeader(std::istream& inputStream)
{
    std::string line;
    
    while (std::getline(inputStream, line))
    {
        std::istringstream lineStream(line);
        std::string token;
        
        lineStream >> token;
    
        if (token == "ply" || token == "PLY" || token == "")
        {
            continue;
        }
        else if (token == "comment")    continue;
        else if (token == "format")     readFormat(lineStream);
        else if (token == "element")    readElement(lineStream);
        else if (token == "property")   readProperty(lineStream);
        else if (token == "obj_info")   continue;
        else if (token == "end_header") break;
        else return false;
    }
    
    return true;    
}

void PLYLoader::readFormat(std::istream& lineStream)
{
    std::string formatString;
    lineStream >> formatString;
    
    if (formatString == "binary_little_endian")
        mIsBinary = true;
    else if (formatString == "binary_big_endian")
        mIsBinary = mIsBigEndian = true;    
}

void PLYLoader::readElement(std::istream& lineStream)
{
    std::string name;
    size_t count;
    lineStream >> name >> count;
    mElements.emplace_back(name, count);
}

void PLYLoader::readProperty(std::istream& lineStream)
{
    std::string name;
    std::string type;
    PLYProperty property;
    
    lineStream >> type;
    
    if(type == "list")
    {
        property.mIsList = true;
        
        std::string countType;
        lineStream >> countType >> type;
        
        property.mListType = readPropertyType(countType);
    }
    
    property.mType = readPropertyType(type);
    property.mTypeSize = property.typeSizes[static_cast<int>(property.mType)];
    
    lineStream >> property.mName;
        
    mElements.back().mProperties.push_back(property);
}

PLYProperty::Type PLYLoader::readPropertyType(const std::string& stringType)
{
    if (stringType == "int8" || stringType == "char")             return PLYProperty::Type::INT8;
    else if (stringType == "uint8" || stringType == "uchar")      return PLYProperty::Type::UINT8;
    else if (stringType == "int16" || stringType == "short")      return PLYProperty::Type::INT16;
    else if (stringType == "uint16" || stringType == "ushort")    return PLYProperty::Type::UINT16;
    else if (stringType == "int32" || stringType == "int")        return PLYProperty::Type::INT32;
    else if (stringType == "uint32" || stringType == "uint")      return PLYProperty::Type::UINT32;
    else if (stringType == "float32" || stringType == "float")    return PLYProperty::Type::FLOAT32;
    else if (stringType == "float64" || stringType == "double")   return PLYProperty::Type::DOUBLE;
    return PLYProperty::Type::INVALID;    
}

void PLYLoader::readASCIIProperty(PLYProperty::Type type, std::istream& inputStream, std::vector<glm::byte>& destination, size_t offset)
{
    if(type == PLYProperty::Type::INT8) castASCIIProperty(readASCII<int32_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::UINT8) castASCIIProperty(readASCII<uint32_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::INT16) castASCIIProperty(readASCII<int16_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::UINT16) castASCIIProperty(readASCII<uint16_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::INT32) castASCIIProperty(readASCII<int32_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::UINT32) castASCIIProperty(readASCII<uint32_t>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::FLOAT32) castASCIIProperty(readASCII<float>(inputStream), destination, offset);
    else if (type == PLYProperty::Type::DOUBLE) castASCIIProperty(readASCII<double>(inputStream), destination, offset);
}



void PLYLoader::readData(std::istream& inputStream)
{
    std::array<std::vector<glm::byte>, 6> meshData;
    glm::vec3 max(std::numeric_limits<float>::min());
    glm::vec3 min(std::numeric_limits<float>::max());
    std::vector<std::array<size_t, 4>> vertexCategories;
    auto colorCategoryIndex = 0;
    bool addAlphaToColor = false;
    bool colorToFloat = false;
                
    for(const auto & element: mElements)
    {
        if(element.mName == "vertex")
        {            
            size_t elementSize = 0;
            
            for(const auto& property: element.mProperties)
            {
                if(property.mName == "x") vertexCategories.push_back({GLBuffer::Vertex, elementSize, 0, 0});
                else if(property.mName == "nx") vertexCategories.push_back({GLBuffer::Normal, elementSize, 0, 0});
                else if(property.mName == "s" || property.mName == "texture_u") vertexCategories.push_back({GLBuffer::UV, elementSize, 0, 0});
                else if(property.mName == "red")
                {
                    if(static_cast<int>(property.mType) < static_cast<int>(PLYProperty::Type::FLOAT32))
                        colorToFloat = true;                        

                    colorCategoryIndex = static_cast<int>(vertexCategories.size());    
                    vertexCategories.push_back({GLBuffer::Color, elementSize, 0, 0});
                }
                
                vertexCategories.back()[2] += property.mTypeSize;
                vertexCategories.back()[3]++;
                
                elementSize += property.mTypeSize;
            }

            if(vertexCategories[colorCategoryIndex][3] < 4)
                addAlphaToColor = true;            
            
            for(size_t i = 0; i < element.mCount; ++i)
            {
                std::vector<glm::byte> elementData(elementSize, 0);
                auto offset = 0;
                
                for(const auto& property: element.mProperties)
                {
                    readASCIIProperty(property.mType, inputStream, elementData, offset);                    
                    offset += property.mTypeSize;
                }
                
                for(const auto& category: vertexCategories)
                {                                                            
                    if(category[0] == GLBuffer::Vertex)
                    {
                        glm::vec3 vertex;
                        memcpy(glm::value_ptr(vertex), elementData.data() + category[1], category[2]);
                        min = glm::min(vertex, min);
                        max = glm::max(vertex, max);
                    }

                    std::copy(begin(elementData) + category[1], begin(elementData) + category[1] + category[2], std::back_inserter(meshData[category[0]]));
                }              
            }
        }
        else if(element.mName == "face")
        {
            std::array<std::vector<glm::byte>, 6> newMeshData;

            std::vector<std::array<size_t, 2>> propertyCategories;
            bool useNewData = false;
            size_t elementSize = 0;
            
            for(const auto& property: element.mProperties)
            {
                if(property.mName == "vertex_indices") propertyCategories.push_back({GLBuffer::Index, 0});
                else if(property.mName == "texcoord")
                {
                    useNewData = true;
                    propertyCategories.push_back({GLBuffer::UV, 0});
                }
                else if(property.mName == "red")
                {
                    useNewData = true;
                    if(static_cast<int>(property.mType) < static_cast<int>(PLYProperty::Type::FLOAT32))
                        colorToFloat = true;
                    
                    colorCategoryIndex = static_cast<int>(propertyCategories.size());
                    propertyCategories.push_back({GLBuffer::Color, 0});
                }
                
                propertyCategories.back()[1]++;
                
                elementSize += property.mTypeSize;
            }            
            
            if(propertyCategories[colorCategoryIndex][1] < 4)
                addAlphaToColor = true;            
            
            auto newIndexCount = 0;
            
            for(size_t i = 0; i < element.mCount; ++i)
            {
                auto propertyIndex = 0;
                std::array<int32_t, 3> vertexIndices;                
                
                for(auto j = 0; j < propertyCategories.size(); ++j)
                {
                    auto category = propertyCategories[j];
                    auto property = element.mProperties[propertyIndex];
                    auto propertySize = property.mTypeSize;
                                        
                    uint32_t count = 1;
                    
                    if(property.mIsList)
                    {
                        inputStream >> count;
                        propertySize *= count;                        
                    }
                    
                    std::vector<glm::byte> elementData(propertySize, 0);
                    auto offset = 0;
                    
                    for(int i = 0; i < count; ++i)
                    {
                        readASCIIProperty(property.mType, inputStream, elementData, offset);
                        offset += property.mTypeSize;
                    }
                                        
                    if(useNewData)
                    {
                        if(category[0] == GLBuffer::Index)
                        {
                            memcpy(vertexIndices.data(), elementData.data(), propertySize);
                            
                            std::vector<int32_t> newIndices(vertexIndices.size());
                            
                            for(int idx = 0; idx < vertexIndices.size(); ++idx)
                            {
                                for(const auto& vertexCategory: vertexCategories)
                                    std::copy(begin(meshData[vertexCategory[0]]) + vertexIndices[idx] * vertexCategory[2], begin(meshData[vertexCategory[0]]) + (vertexIndices[idx] + 1) * vertexCategory[2], std::back_inserter(newMeshData[vertexCategory[0]]));
                                
                                newIndices[idx] = newIndexCount++;
                            }
                            
                            auto rawNewIndices = reinterpret_cast<glm::byte*>(newIndices.data());
                            
                            std::copy(rawNewIndices, rawNewIndices + newIndices.size() * sizeof(int32_t), std::back_inserter(newMeshData[category[0]]));
                        }
                        else
                            std::copy(begin(elementData), end(elementData), std::back_inserter(newMeshData[category[0]]));
                    }
                    else
                        std::copy(begin(elementData), end(elementData), std::back_inserter(meshData[category[0]]));

                    propertyIndex += category[1];
                }                           
            }
           
            if(useNewData)
                meshData = newMeshData;
        }            
    }
    
//     std::vector<float> vertices(meshData[GLBuffer::Vertex].size() / (sizeof(float)));
//     std::vector<float> normals(meshData[GLBuffer::Normal].size() / (sizeof(float)));
//     std::vector<float> uvs(meshData[GLBuffer::UV].size() / (sizeof(float)));
//     std::vector<int32_t> indices(meshData[GLBuffer::Index].size() / (sizeof(int32_t)));
//     std::vector<float> colors(meshData[GLBuffer::Color].size() / (sizeof(float)));
//     
//     memcpy(vertices.data(), meshData[GLBuffer::Vertex].data(), meshData[GLBuffer::Vertex].size());
//     memcpy(normals.data(), meshData[GLBuffer::Normal].data(), meshData[GLBuffer::Normal].size());
//     memcpy(uvs.data(), meshData[GLBuffer::UV].data(), meshData[GLBuffer::UV].size());
//     memcpy(indices.data(), meshData[GLBuffer::Index].data(), meshData[GLBuffer::Index].size());
//     memcpy(colors.data(), meshData[GLBuffer::Color].data(), meshData[GLBuffer::Color].size());
    
    auto mesh = mResourceManager->createMesh("objMesh");
    mesh->updateData(GLBuffer::Vertex, 0, meshData[GLBuffer::Vertex].size(), meshData[GLBuffer::Vertex].data());
    if(meshData[GLBuffer::Normal].size())
        mesh->updateData(GLBuffer::Normal, 0, meshData[GLBuffer::Normal].size(), meshData[GLBuffer::Normal].data());
    if(meshData[GLBuffer::UV].size())
        mesh->updateData(GLBuffer::UV, 0, meshData[GLBuffer::UV].size(), meshData[GLBuffer::UV].data());
    if(meshData[GLBuffer::Color].size())
    {
        if(colorToFloat)
        {
            auto rawColor = reinterpret_cast<uint32_t*>(meshData[GLBuffer::Color].data());
            auto colorSize = meshData[GLBuffer::Color].size()/sizeof(uint32_t);
            if(addAlphaToColor) colorSize = 4 * colorSize / 3;
            
            std::vector<float> floatColorData(colorSize, 1.0);
            
            if(addAlphaToColor)                
                for(auto i = 0, j = 0; j < colorSize; i += 3, j += 4)
                {
                    floatColorData[j] = rawColor[i]/255.0;
                    floatColorData[j + 1] = rawColor[i + 1]/255.0;
                    floatColorData[j + 2] = rawColor[i + 2]/255.0;
                }
            else
                for(auto i = 0; i < colorSize; i++)
                    floatColorData[i] = rawColor[i]/255.0;
            
            mesh->updateData(GLBuffer::Color, 0, floatColorData.size() * sizeof(float), floatColorData.data());                
        }
        else
        {
            if(addAlphaToColor)
            {
                auto rawColor = reinterpret_cast<float*>(meshData[GLBuffer::Color].data());
                auto colorSize = (4 * meshData[GLBuffer::Color].size())/(3 *sizeof(float));
                std::vector<float> floatColorData(colorSize, 1.0);
                
                for(auto i = 0, j = 0; j < colorSize; i += 3, j += 4)
                {
                    floatColorData[j] = rawColor[i];
                    floatColorData[j + 1] = rawColor[i + 1];
                    floatColorData[j + 2] = rawColor[i + 2];
                }
                
                mesh->updateData(GLBuffer::Color, 0, floatColorData.size() * sizeof(float), floatColorData.data());
            }
            else
                mesh->updateData(GLBuffer::Color, 0, meshData[GLBuffer::Color].size(), meshData[GLBuffer::Color].data());
        }
    }
    else
    {
        std::vector<float> meshColors;   
        meshColors.resize(4 * meshData[GLBuffer::Vertex].size()/3, 1.0f);
        mesh->updateData(GLBuffer::Color, 0, meshColors.size() * sizeof(float), meshColors.data());
    }
    mesh->updateData(GLBuffer::Index, 0, meshData[GLBuffer::Index].size(), meshData[GLBuffer::Index].data());
    mesh->updateSubMeshData({static_cast<unsigned int>(meshData[GLBuffer::Index].size()/sizeof(int32_t))});

    mesh->setBoundingBox(min, max);

    auto model = mResourceManager->createModel("objModel");
    auto material = mResourceManager->createMaterial("objMaterial", "ProjectiveTex");
    model->setMesh(mesh);
    model->insertMaterial(material);

    mScene->insertModel(model);

    auto camera = mResourceManager->createCamera("objCamera", model);

    mScene->insertCamera(camera);
    mScene->setProjCameraNeedUpdate(true);
    mScene->setViewCameraNeedUpdate(true);  
}


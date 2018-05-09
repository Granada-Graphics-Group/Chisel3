#include "objloader.h"
#include "resourcemanager.h"
#include "scene3d.h"

#include <fstream>
#include <glm/glm.hpp>


OBJLoader::OBJLoader(ResourceManager* manager, Scene3D* scene, std::string filePath)
:
    mManager(manager),
    mScene(scene)
{
    loadScene(filePath);
}



// *** Public Methods *** //

void OBJLoader::loadScene(std::string filePath)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec4> colors;
    
    std::vector<float> meshVertices;
    std::vector<float> meshNormals;
    std::vector<float> meshUVs;
    std::vector<float> meshColors;
    std::vector<uint32_t> meshIndices;

    glm::vec3 max(std::numeric_limits<float>::min());
    glm::vec3 min(std::numeric_limits<float>::max());
    
    bool validOBJFile = true;
    
    std::ifstream file;
    file.open(filePath, std::ios::in);

    if(file.good())
    {        
        while (!file.eof() && validOBJFile)
        {
            std::string line;
            std::vector<std::string> words;

            line.clear();
            std::getline(file, line);
            extractWords(words, line, " ");

            switch (line[0])
            {
            case 'v':
                switch (line[1])
                {
                case ' ':
                {
                    auto x = std::stof(words[1]);
                    auto y = std::stof(words[2]);
                    auto z = std::stof(words[3]);
                    positions.push_back(glm::vec3(x, y, z));

                    if (x >  max.x)
                        max.x = x;
                    else if (x < min.x)
                        min.x = x;

                    if (y >  max.y)
                        max.y = y;
                    else if (y < min.y)
                        min.y = y;

                    if (z >  max.z)
                        max.z = z;
                    else if (z < min.z)
                        min.z = z;

                    if (words.size() > 4)
                    {
                        auto r = std::stof(words[4]);
                        auto g = std::stof(words[5]);
                        auto b = std::stof(words[6]);
                        
                        auto a = (words.size() == 8) ? std::stof(words[7]) : 1.0f;
                        
                        colors.push_back(glm::vec4(r, g, b, a));
                    }
                }
                break;
                case 'n':
                {
                    auto nx = std::stof(words[1]);
                    auto ny = std::stof(words[2]);
                    auto nz = std::stof(words[3]);
                    normals.push_back(glm::vec3(nx, ny, nz));
                }
                break;
                case 't':
                {
                    auto cs = std::stof(words[1]);
                    auto ct = std::stof(words[2]);
                    textureCoords.push_back(glm::vec2(cs, ct));
                }
                break;
                default:
                    ;
                }
                break;
            case 'f':
            {
                for(auto i = 1; i < words.size(); ++i)
                {
                    std::vector<std::string> vertex;
                    extractWords(vertex, words[i], "/");

                    auto index = std::stoul(vertex[0]) - 1;
                    auto position = positions[index];                    
                    std::copy(begin(position), end(position), std::back_inserter(meshVertices));

                    if(textureCoords.size())
                    {
                        auto texIndex = std::stoul(vertex[1]) - 1;
                        auto texCoord = textureCoords[texIndex];
                        std::copy(begin(texCoord), end(texCoord), std::back_inserter(meshUVs));
                    }
                    
                    if(normals.size())
                    {
                        auto normIndex = std::stoul(vertex[2]) - 1;
                        auto normal = normals[normIndex];                    
                        std::copy(begin(normal), end(normal), std::back_inserter(meshNormals));
                    }
                                        
                    if(colors.size())
                    {
                        auto color = colors[index];
                        std::copy(begin(color), end(color), std::back_inserter(meshColors));
                    }
                    
                    meshIndices.push_back(static_cast<uint32_t>(meshIndices.size()));
                }
            }
            break;
            case 'm':
                //cargarMaterialesOBJ(miModelo, words[1]);
                ;
            }
        }
        
        file.close();
        
        auto mesh = mManager->createMesh("objMesh");
        mesh->updateData(GLBuffer::Vertex, 0, meshVertices.size() * sizeof(float), meshVertices.data());
        if(meshNormals.size())
            mesh->updateData(GLBuffer::Normal, 0, meshNormals.size() * sizeof(float), meshNormals.data());
        if(meshUVs.size())
            mesh->updateData(GLBuffer::UV, 0, meshUVs.size() * sizeof(float), meshUVs.data());        
        if(meshColors.size())
            mesh->updateData(GLBuffer::Color, 0, meshColors.size() * sizeof(float), meshColors.data());
        else
        {
            meshColors.resize(4 * meshVertices.size()/3, 1.0f);
            mesh->updateData(GLBuffer::Color, 0, meshColors.size() * sizeof(float), meshColors.data());
        }
        mesh->updateData(GLBuffer::Index, 0, meshIndices.size() * sizeof(uint32_t), meshIndices.data());
        mesh->updateIndexData(meshIndices);
        mesh->updateSubMeshData({static_cast<unsigned int>(meshIndices.size())});
        
        mesh->setBoundingBox(min, max);

        auto model = mManager->createModel("objModel");        
        auto material = mManager->createMaterial("objMaterial", "ProjectiveTex");        
        model->setMesh(mesh);
        model->insertMaterial(material);        
        
        mScene->insertModel(model);
        
        auto camera = mManager->createCamera("objCamera", model);
                
        mScene->insertCamera(camera);
        mScene->setProjCameraNeedUpdate(true);
        mScene->setViewCameraNeedUpdate(true);
    }
    else
        validOBJFile = false;
}


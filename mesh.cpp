#include "mesh.h"
#include "scene3d.h"
#include "logger.hpp"
#include "bufferlockmanager.h"

#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>


HalfEdge::HalfEdge()
:
    mNext(nullptr), mPrevious(nullptr), mPair(nullptr), mOrigin(nullptr), mLeft(nullptr)
{

}


Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord)
:
    mPosition(position),
    mNormal(normal),
    mTextureCoord(textureCoord)
{

}

Face::Face(uint32_t index1, uint32_t index2, uint32_t index3, HalfEdge* he)
:
    mIndices(index1, index2, index3),
    mHE(he)
{

}


Mesh::Mesh(std::string name)
:
    mName(name),
    mBufferData(GLBuffer::Count),
    mBufferUpdateSize(GLBuffer::Count, 0),
    mBufferUpdateOffset(GLBuffer::Count, 0),
    mBufferUpdateSizeState(GLBuffer::Count, SizeState::Equal)
{

}

Mesh::~Mesh()
{
//     mVertexBufferObject->dispose();    
//     mNormalBufferObject->dispose();
//     mUVBufferObject->dispose();
//     mIndexBufferObject->dispose();
}

void Mesh::insertVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord)
{
    mVertices.push_back(new Vertex(position, normal, textureCoord));
}

void Mesh::insertFace(uint32_t index1, uint32_t index2, uint32_t index3)
{
    HalfEdge *faceHalfEdges[3] = {0, 0, 0};
    faceHalfEdges[0] = new HalfEdge();
    faceHalfEdges[1] = new HalfEdge();
    faceHalfEdges[2] = new HalfEdge();


    faceHalfEdges[0]->setOrigin(mVertices[index1]);
    faceHalfEdges[0]->setNext(faceHalfEdges[1]);

    uint64_t vertIndex1_64 = index1;
    uint64_t vertIndex2_64 = index2;
    uint64_t vertIndex3_64 = index3;
    
    uint64_t key = static_cast<int64_t>(index1) | (static_cast<int64_t>(index2) << 32);

    map[key] = faceHalfEdges[0];
    mHalfEdges.push_back(faceHalfEdges[0]);

    faceHalfEdges[1]->setOrigin(mVertices[index2]);
    faceHalfEdges[1]->setNext(faceHalfEdges[2]);
    key = static_cast<int64_t>(index2) | (static_cast<int64_t>(index3) << 32);
    map[key] = faceHalfEdges[1];
    mHalfEdges.push_back(faceHalfEdges[1]);

    faceHalfEdges[2]->setOrigin(mVertices[index3]);
    faceHalfEdges[2]->setNext(faceHalfEdges[0]);
    key = static_cast<int64_t>(index3) | (static_cast<int64_t>(index1) << 32);
    map[key] = faceHalfEdges[2];
    mHalfEdges.push_back(faceHalfEdges[2]);


    mFaces.push_back(new Face(index1, index2, index3, faceHalfEdges[0]));
}

void Mesh::setBoundingBox(glm::vec3 min, glm::vec3 max)
{
    mBoundingBox.min = min;
    mBoundingBox.max = max;
    mBoundingBox.center = (max + min) / 2.0f;
}

bool Mesh::resizeBufferCount()
{
    bool resize = false;
    
    if(mBufferData.size() < GLBuffer::Count)
    {
        mBufferData.resize(GLBuffer::Count);
        resize = true;
    }
    
    return resize;
}


void Mesh::generateAdjacencyInformation()
{
    if (map.size() != 3 * mFaces.size())
        LOG_WARN("Mesh::generateAdjacencyInformation: duplicated egdes are possible");

    unsigned long borderEdges = 0;


    for(auto mapped: map)
    {
        uint64_t currenHEIndex = mapped.first;
        uint64_t pairIndex = ((currenHEIndex & 0xffffffff) << 32) | (currenHEIndex >> 32);
        HalfEdge* currentHE = mapped.second;
        HalfEdge* pairHE = map[pairIndex];

        if (pairHE && currentHE)
        {
            pairHE->setPair(currentHE);
            currentHE->setPair(pairHE);
        }
        else
        {
            LOG_WARN("Mesh::generateAdjacencyInformation: ", currenHEIndex, " has no pair");            
            borderEdges++;
        }
    }

/*    qDebug()<< "Numero limítrofes: " << numeroLimitrofes;

    if (numeroLimitrofes)
    {
        for (unsigned long long indiceCara = 0; indiceCara < mFaces.size(); indiceCara++)
        {
            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 2]->origen());

            if (vecHE[3 * indiceCara + 2]->pareja())
                verticesRepresentacion.push_back(vecHE[3 * indiceCara + 2]->pareja()->siguiente()->siguiente()->origen());
            else
                verticesRepresentacion.push_back(verticesRepresentacion.back());

            verticesRepresentacion.push_back(vecHE[3 * indiceCara]->origen());

            if (vecHE[3 * indiceCara]->pareja())
                verticesRepresentacion.push_back(vecHE[3 * indiceCara]->pareja()->siguiente()->siguiente()->origen());
            else
                verticesRepresentacion.push_back(verticesRepresentacion[verticesRepresentacion.size()-2]);

            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 1]->origen());

            if (vecHE[3 * indiceCara + 1]->pareja())
                verticesRepresentacion.push_back(vecHE[3 * indiceCara + 1]->pareja()->siguiente()->siguiente()->origen());
            else
                verticesRepresentacion.push_back(verticesRepresentacion[verticesRepresentacion.size()-3]);
        }
    }
    else
    {

        for (unsigned long long indiceCara = 0; indiceCara < mFaces.size(); indiceCara ++)
        {
            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 2]->origen());
            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 2]->pareja()->siguiente()->siguiente()->origen());
            verticesRepresentacion.push_back(vecHE[3 * indiceCara]->origen());
            verticesRepresentacion.push_back(vecHE[3 * indiceCara]->pareja()->siguiente()->siguiente()->origen());
            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 1]->origen());
            verticesRepresentacion.push_back(vecHE[3 * indiceCara + 1]->pareja()->siguiente()->siguiente()->origen());
        }
    }

    for (unsigned long long i=0; i < verticesRepresentacion.size(); i++)
    {
        vertices.push_back(verticesRepresentacion[i]->vertice().x);
        vertices.push_back(verticesRepresentacion[i]->vertice().y);
        vertices.push_back(verticesRepresentacion[i]->vertice().z);

        //qDebug() << "Vertice[" << i << "]: " << vecVertices[heRepresentacion[i]]->vertice().x << " " << vecVertices[heRepresentacion[i]]->vertice().y << " " << vecVertices[heRepresentacion[i]]->vertice().z;

        normales.push_back(verticesRepresentacion[i]->normal().x);
        normales.push_back(verticesRepresentacion[i]->normal().y);
        normales.push_back(verticesRepresentacion[i]->normal().z);

        //qDebug() << "Normal[" << i << "]: " << vecVertices[heRepresentacion[i]]->normal().x << " " << vecVertices[heRepresentacion[i]]->normal().y << " " << vecVertices[heRepresentacion[i]]->normal().z;

        coordTextura.push_back(verticesRepresentacion[i]->coordenadaTextura().x);
        coordTextura.push_back(verticesRepresentacion[i]->coordenadaTextura().y);
    }*/

    /*qDebug() << "Vertice: " << mFaces.back()->halfEdge()->origen()->vertice().x << " " << mFaces.back()->halfEdge()->origen()->vertice().y << " " << mFaces.back()->halfEdge()->origen()->vertice().z;

    qDebug() << "Vertice: " << mFaces.back()->halfEdge()->origen()->normal().x << " " << mFaces.back()->halfEdge()->origen()->normal().y << " " << mFaces.back()->halfEdge()->origen()->normal().z;

    qDebug() << "Vertice: " << mFaces.back()->halfEdge()->origen()->coordenadaTextura().x << " " << mFaces.back()->halfEdge()->origen()->coordenadaTextura().y;*/

}

int Mesh::sceneIndex(Scene3D* scene) const
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    return (search != end(mScenes)) ? (*search).second : -1;
}


void Mesh::updateSubMeshData(const std::vector< GLuint >& newData, int offset)
{
    //mSubMeshes = newData;
    if((newData.size() + offset) > mSubMeshes.size())
        mSubMeshes.resize(newData.size() + offset);
    std::copy(begin(newData), end(newData), begin(mSubMeshes) + offset);
}

void Mesh::clearUpdatedBuffers()
{
    for(auto updatedBuffer : mUpdatedBuffers)
        mBufferUpdateSizeState[updatedBuffer] = SizeState::Equal;
    
    mUpdatedBuffers.clear();
}


void Mesh::updateData(const int buffer, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
        
    if((size + offset) > mBufferData[buffer].size())
    {
        mBufferData[buffer].resize(size + offset);
        mBufferUpdateSizeState[buffer] = SizeState::Bigger;
        for(auto scene : mScenes) (scene.first)->setMeshesDirtyAfter(this);
    }
    else
    {
        mBufferUpdateSizeState[buffer] = SizeState::Equal;
        for(auto scene : mScenes) (scene.first)->setMeshDirty(this);
    }
        
    std::copy(byteData, byteData + size, begin(mBufferData[buffer]) + offset);
    
    mBufferUpdateSize[buffer] = size;
    mBufferUpdateOffset[buffer] = offset;
    
    if(buffer != GLBuffer::Index2)
        mUpdatedBuffers.insert(buffer);
    else
        mUpdatedBuffers.insert(GLBuffer::Index);
}

void Mesh::updateVertexData(const std::vector< GLfloat >& newData, int offset)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(newData.data());
    auto updateSize = newData.size() * sizeof(float);

    updateData(GLBuffer::Vertex, 0, updateSize, byteData);
}

void Mesh::updateNormalData(const std::vector< GLfloat >& newData, int offset)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(newData.data());
    auto updateSize = newData.size() * sizeof(float);
    
    updateData(GLBuffer::Normal, 0, updateSize, byteData);
}

void Mesh::updateUVData(const std::vector< GLfloat >& newData, int offset)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(newData.data());
    auto updateSize = newData.size() * sizeof(float);

    updateData(GLBuffer::UV, 0, updateSize, byteData);
}

void Mesh::updateIndexData(const std::vector< GLuint >& newData, int offset)
{
    //mIndexBufferData = newData;
//     if((newData.size() + offset) > mIndexBufferData.size())
//         mIndexBufferData.resize(newData.size() + offset);
//     std::copy(begin(newData), end(newData), begin(mIndexBufferData) + offset);
    
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(newData.data());
    auto updateSize = newData.size() * sizeof(GLuint);

    updateData(GLBuffer::Index, 0, updateSize, byteData);       
}

void Mesh::setBufferDirty(const int buffer)
{
    mBufferUpdateSize[buffer] = mBufferData[buffer].size();
    mBufferUpdateOffset[buffer] = 0;

    mUpdatedBuffers.insert(buffer);    
}

void Mesh::useIndexBuffer2(bool use)
{
    mUseIndexBuffer2 = use;
    
    if(!mUseIndexBuffer2)
    {
        setBufferDirty(GLBuffer::Index);
    }
}

bool Mesh::addScene(Scene3D* scene, int index)
{
    if(std::find(begin(mScenes), end(mScenes), std::pair<Scene3D*, int>(scene, index)) == end(mScenes))
    {
        mScenes.push_back({scene, index});
        return true;
    }
    else
        return false;
}

bool Mesh::removeScene(Scene3D* scene)
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    if(search != end(mScenes))
    {
        mScenes.erase(search);
        return true;
    }
    else
        return false;
}


void Mesh::setVAO()
{    
    //LOG("Setting VAO: " << std::endl;
    //LOG("Vertex: " << mVertexBufferData.size() << std::endl;
    //LOG("Normals: " << mNormalBufferData.size() << std::endl;
    //LOG("UVs: " << mUVBufferData.size() << std::endl;
    //LOG("Index: " << mIndexBufferData.size() << std::endl;
    
    
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
/*    mVertexBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mVertexBufferData.size() * sizeof(GLfloat));
    mVertexBufferObject->bind();
    std::copy(mVertexBufferData.begin(), mVertexBufferData.end(), reinterpret_cast<GLfloat *>(mVertexBufferObject->dataPointer()));
    
    //memcpy(mVertexBufferObject->dataPointer(), &mVertexBufferData.front(), mVertexBufferData.size() * sizeof(GLfloat));

    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(0);
    
    
    mNormalBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mNormalBufferData.size());
    mNormalBufferObject->bind();
    std::copy(mNormalBufferData.begin(), mNormalBufferData.end(), reinterpret_cast<GLfloat *>(mNormalBufferObject->dataPointer()));
    //memcpy(mNormalBufferObject->dataPointer(), &mNormalBufferData.front(), mNormalBufferData.size() * sizeof(GLfloat));

    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(1);

    
    mUVBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mUVBufferData.size() * sizeof(GLfloat));
    mUVBufferObject->bind();
    //memcpy(mUVBufferObject->dataPointer(), &mUVBufferData.front(), mUVBufferData.size() * sizeof(GLfloat));
    std::copy(mUVBufferData.begin(), mUVBufferData.end(), reinterpret_cast<GLfloat *>(mUVBufferObject->dataPointer()));

    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(2);

    mIndexBufferObject = new PersistentBufferObject(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() * sizeof(GLuint));
    mIndexBufferObject->bind();
    //memcpy(mIndexBufferObject->dataPointer(), &mIndexBufferData.front(), mIndexBufferData.size() * sizeof(GLuint));
    std::copy(mIndexBufferData.begin(), mIndexBufferData.end(), reinterpret_cast<GLuint *>(mIndexBufferObject->dataPointer()));
    
    //glGenBuffers(1, &mVBO[3]);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[3]);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() *sizeof(GLuint), &mIndexBufferData.front(), GL_STATIC_DRAW);
    
    //LOG("PBO Vertex: " << mVertexBufferObject->id() << ", cap: " << mVertexBufferObject->capacity() << std::endl;
    //LOG("PBO Normals: " << mNormalBufferObject->id() << ", cap: " << mNormalBufferObject->capacity() << std::endl;
    //LOG("PBO UVs: " << mUVBufferObject->id() << ", cap: " << mUVBufferObject->capacity() << std::endl;
    //LOG("PBO Indexes: " << mIndexBufferObject->id() << ", cap: " << mIndexBufferObject->capacity() << std::endl;
    
    glBindVertexArray(0);*/
    
    /*LOG("VAO: vertices" << std::endl;
    for(auto vert: mVertexBufferData)
        LOG(vert << " ";
    LOG(std::endl;

    
    LOG("VAO: indices" << std::endl;
    for(auto index: mIndexBufferData)
        LOG(index << " ";
    LOG(std::endl;*/
    
    /*float* vertices = new float[18];
    float* coordTextura = new float[12];
  
    vertices[0] = -1.0; vertices[1] = -1.0; vertices[2] = 0.0; 
    vertices[3] = -1.0; vertices[4] = 5.0; vertices[5] = 0.0; 
    vertices[6] = 5.0; vertices[7] = 5.0; vertices[8] = 0.0; 
  
    vertices[9] = 5.0; vertices[10] = -1.0; vertices[11] = 0.0; 
    vertices[12] = -1.0; vertices[13] = -1.0; vertices[14] = 0.0; 
    vertices[15] = 5.0; vertices[16] = 5.0; vertices[17] = 0.0; 
    
    coordTextura[0] = 0.0; coordTextura[1] = 0.0;
    coordTextura[2] = 0.0; coordTextura[3] = 1.0;
    coordTextura[4] = 1.0; coordTextura[5] = 1.0;

    coordTextura[6] = 1.0; coordTextura[7] = 0.0;
    coordTextura[8] = 0.0; coordTextura[9] = 0.0;
    coordTextura[10] = 1.0; coordTextura[11] = 1.0;
    
    GLuint indices[6] = {0,1,2,3,4,5};
    */
    
    
/*    glGenBuffers(1, &mVBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, mVertexBufferData.size() *sizeof(float), &mVertexBufferData.front(), GL_STATIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &mVBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, mNormalBufferData.size() *sizeof(float), &mNormalBufferData.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(1);
    
    glGenBuffers(1, &mVBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
    glBufferData(GL_ARRAY_BUFFER, mUVBufferData.size() *sizeof(float), &mUVBufferData.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnableVertexAttribArray(2);
    
    glGenBuffers(1, &mVBO[3]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() *sizeof(GLuint), &mIndexBufferData.front(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    LOG("VBO vertices size: ", mVBO[0]);
    LOG("VBO Normals size: ", mVBO[1]);
    LOG("VBO UVs size: ", mVBO[2]);
    LOG("VBO Indexes size: ", mVBO[3]);*/
}



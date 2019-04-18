#include "mesh.h"
#include "scene3d.h"
#include "logger.hpp"
#include "bufferlockmanager.h"

#include <string>
#include <cstring>
#include <algorithm>
#include <iostream>

#include <glm/gtx/hash.hpp>


Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord)
:
    mPosition(position),
    mNormal(normal),
    mTextureCoord(textureCoord)
{

}

Face::Face(const std::array<uint32_t, 3>& vertexIndices, HalfEdge* he)
:
    mVertexIndices(vertexIndices),
    mHE(he)
{

}


Mesh::Mesh(std::string name)
:
    mName(name),
    mDataBuffers(GLBuffer::Count),
    mBufferUpdateSize(GLBuffer::Count, 0),
    mBufferUpdateOffset(GLBuffer::Count, 0),
    mBufferUpdateSizeState(GLBuffer::Count, SizeState::Equal)
{

}

Mesh::Mesh::Mesh(const Mesh& mesh)
{
    mName = mesh.name();

    mSubMeshes = mesh.subMeshes();    
    mVertices = mesh.vertices();    
    
    mDataBuffers = mesh.buffers();
    
    mBufferUpdateSize = std::vector<std::size_t>(GLBuffer::Count, 0);
    mBufferUpdateOffset = std::vector<std::size_t>(GLBuffer::Count, 0);
    mBufferUpdateSizeState = std::vector<SizeState>(GLBuffer::Count, SizeState::Equal);
    
    mBoundingBox = mesh.boundingBox();
    
    //std::vector<std::unique_ptr<HalfEdge>> mHalfEdges;
    //std::vector<std::unique_ptr<Face>> mFaces;
    
//     mVertexBufferObject;
//     mNormalBufferObject;
//     mUVBufferObject;
//     mIndexBufferObject;
    
    //mDataBuffers = mesh.bufferDa;
/*    std::vector<std::size_t> mBufferUpdateSize;
    std::vector<std::size_t> mBufferUpdateOffset;
    std::vector<SizeState> mBufferUpdateSizeState;     
    std::unordered_set<unsigned int> mUpdatedBuffers;
    bool mUseIndexBuffer2 = false;
    
    std::vector<std::pair<Scene3D*, int>> mScenes;
    
    BoundingBox mBoundingBox;  */  
}

Mesh::~Mesh()
{
}

void Mesh::insertVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord)
{
    mVertices.push_back(new Vertex(position, normal, textureCoord));
}

void Mesh::insertFace(uint32_t index1, uint32_t index2, uint32_t index3)
{
//     HalfEdge *faceHalfEdges[3] = {0, 0, 0};
//     faceHalfEdges[0] = new HalfEdge();
//     faceHalfEdges[1] = new HalfEdge();
//     faceHalfEdges[2] = new HalfEdge();
// 
// 
//     faceHalfEdges[0]->setOrigin(mVertices[index1]);
//     faceHalfEdges[0]->setNext(faceHalfEdges[1]);
// 
//     uint64_t vertIndex1_64 = index1;
//     uint64_t vertIndex2_64 = index2;
//     uint64_t vertIndex3_64 = index3;
//     
//     uint64_t key = static_cast<int64_t>(index1) | (static_cast<int64_t>(index2) << 32);
// 
//     map[key] = faceHalfEdges[0];
//     mHalfEdges.push_back(faceHalfEdges[0]);
// 
//     faceHalfEdges[1]->setOrigin(mVertices[index2]);
//     faceHalfEdges[1]->setNext(faceHalfEdges[2]);
//     key = static_cast<int64_t>(index2) | (static_cast<int64_t>(index3) << 32);
//     map[key] = faceHalfEdges[1];
//     mHalfEdges.push_back(faceHalfEdges[1]);
// 
//     faceHalfEdges[2]->setOrigin(mVertices[index3]);
//     faceHalfEdges[2]->setNext(faceHalfEdges[0]);
//     key = static_cast<int64_t>(index3) | (static_cast<int64_t>(index1) << 32);
//     map[key] = faceHalfEdges[2];
//     mHalfEdges.push_back(faceHalfEdges[2]);
// 
// 
//     mFaces.push_back(new Face(index1, index2, index3, faceHalfEdges[0]));
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
    
    if(mDataBuffers.size() < GLBuffer::Count)
    {
        mDataBuffers.resize(GLBuffer::Count);
        resize = true;
    }
    
    return resize;
}


bool ptInTriangle(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) 
{
    auto dX = p.x-p2.x;
    auto dY = p.y-p2.y;
    auto dX21 = p2.x-p1.x;
    auto dY12 = p1.y-p2.y;
    auto D = dY12*(p0.x-p2.x) + dX21*(p0.y-p2.y);
    auto s = dY12*dX + dX21*dY;
    auto t = (p2.y-p0.y)*dX + (p0.x-p2.x)*dY;
    if (D<0) return s<=0 && t<=0 && s+t>=D;
    return s>=0 && t>=0 && s+t<=D;
}

double cross(glm::dvec2 const& a, glm::dvec2 const& b)
{
    return a.x * b.y - b.x * a.y;
}

bool sameSide(glm::dvec2 p1, glm::dvec2 p2, glm::dvec2 a, glm::dvec2 b)
{
//     auto cp1 = cross(b - a, p1 - a);
//     auto cp2 = cross(b - a, p2 - a);
// 
//     return glm::dot(cp1, cp2) >= 0;
    
    auto dA = (a.x - p1.x) * (p2.y - p1.y) - (a.y - p1.y) * (p2.x - p1.x);
    auto dB = (b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x);
    
    return ((dA > 0 && dB > 0) || (dA < 0 && dB < 0)) ? true : false;
    
}

glm::vec2 computePoint(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c, glm::dvec2 d, glm::dvec2 e, glm::dvec2 g)
{
    auto lengthAB = glm::length(b - a);
    auto lengthAC = glm::length(c - a);
    auto lengthDE = glm::length(e - d);
    
    auto lengthDF = (lengthDE * lengthAC) / lengthAB;
    
    auto angle = glm::acos(glm::dot(b - a, c - a) / (lengthAB * lengthAC));
    auto vecDF = glm::normalize(glm::rotate(e - d, angle));
    
    auto pointF = d + vecDF * lengthDF;
    
    if(sameSide(d, e, pointF, g))
    {
        vecDF = glm::normalize(glm::rotate(e - d, -angle));
        pointF = d + vecDF * lengthDF;
    }
    
    return pointF;
}

glm::vec2 computeNormal(glm::dvec2 a, glm::dvec2 b, glm::dvec2 c)
{
    glm::dvec2 distance = b - a;
    glm::dvec2 normal(distance.y, -distance.x);
    
    normal = glm::normalize(normal);
    
    glm::dvec2 testPoint = a + normal * 0.2;
    
    if(sameSide(a, b, testPoint, c))
        normal = glm::normalize(glm::vec2(-distance.y, distance.x));
    
    return normal;
}

void Mesh::generateAdjacencyInformation()
{
    std::unordered_map<glm::vec3, uint32_t> uniqueVertices;
    std::vector<uint32_t> indices;
        
    float* vertexBuffer = reinterpret_cast<float*>(mDataBuffers[GLBuffer::Vertex].data());
    float* uvBuffer = reinterpret_cast<float*>(mDataBuffers[GLBuffer::UV].data());
    auto vertexBufferSize = mDataBuffers[GLBuffer::Vertex].size()/sizeof(float);
    uint32_t* indexBuffer = reinterpret_cast<uint32_t*>(mDataBuffers[GLBuffer::Index].data());
    auto indexBufferSize = mDataBuffers[GLBuffer::Index].size()/sizeof(uint32_t);
    
    for(auto i = 0; i < vertexBufferSize; i += 3)
    {
        glm::vec3 vertex = {vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]};
        
        if(uniqueVertices.count(vertex) == 0)
            uniqueVertices[vertex] = static_cast<uint32_t>(uniqueVertices.size());
        
        indices.push_back(uniqueVertices[vertex]);
        
        //LOG(indices.size() - 1, " - ", indices.back(), " vert(", vertex.x, ", ", vertex.y, ", ", vertex.z, ")    uv(", uvBuffer[2 * i/3], ", ", uvBuffer[2 * i/3  + 1], ")");
    }

    auto uniqueIndexSize = indices.size();
    
    std::unordered_map<uint64_t, HalfEdge *> map;
   
    for(int i = 0; i < indexBufferSize; i += 3)
    {        
        std::array<uint32_t, 3> uniqueIndices = {indices[indexBuffer[i]], indices[indexBuffer[i + 1]], indices[indexBuffer[i + 2]]};
        std::array<uint32_t, 3> bufferIndices = {indexBuffer[i], indexBuffer[i + 1], indexBuffer[i + 2]};
        
        for(int i = 0; i < 3; ++i) mHalfEdges.push_back(std::make_unique<HalfEdge>());        
        mFaces.push_back(std::make_unique<Face>(bufferIndices, mHalfEdges[mHalfEdges.size() - 3].get()));
        
        for(auto i = 0; i < 3; ++i)
        {   
            auto HEIndex = mHalfEdges.size() - 3;
            mHalfEdges[HEIndex + i]->setOrigin(bufferIndices[i]);
            mHalfEdges[HEIndex + i]->setNext(mHalfEdges[HEIndex + (i + 1) % 3].get());
            mHalfEdges[HEIndex + i]->setFace(mFaces.back().get());
            uint64_t key = static_cast<uint64_t>(uniqueIndices[i]) | (static_cast<uint64_t>(uniqueIndices[(i + 1) % 3]) << 32);
            map[key] = mHalfEdges[HEIndex + i].get();            
        }                
    }
    
//     for(int i = 0; i < indices.size(); i += 3)
//     {
//         std::array<uint32_t, 3> uniqueIndices = {indices[i], indices[i + 1], indices[i + 2]};
//         std::array<uint32_t, 3> bufferIndices = {indexBuffer[i], indexBuffer[i + 1], indexBuffer[i + 2]};
//         
//         for(int i = 0; i < 3; ++i) mHalfEdges.push_back(std::make_unique<HalfEdge>());        
//         mFaces.push_back(std::make_unique<Face>(bufferIndices, mHalfEdges[mHalfEdges.size() - 3].get()));
//         
//         for(auto i = 0; i < 3; ++i)
//         {   
//             auto HEIndex = mHalfEdges.size() - 3;
//             mHalfEdges[HEIndex + i]->setOrigin(bufferIndices[i]);
//             mHalfEdges[HEIndex + i]->setNext(mHalfEdges[HEIndex + (i + 1) % 3].get());
//             mHalfEdges[HEIndex + i]->setFace(mFaces.back().get());
//             uint64_t key = static_cast<uint64_t>(uniqueIndices[i]) | (static_cast<uint64_t>(uniqueIndices[(i + 1) % 3]) << 32);
//             map[key] = mHalfEdges[HEIndex + i].get();            
//         }                
//     }

    if (map.size() != 3 * mFaces.size())
        LOG_WARN("Mesh::generateAdjacencyInformation: duplicated egdes are possible");
    
    uint64_t noPairEdgeCount = 0;
//     std::set<std::array<uint32_t, 2>> borderHalfEdges;
//     std::set<std::array<uint32_t, 3>> borderFaces;
    std::vector<uint32_t> borderHalfEdges;
    std::vector<uint32_t> borderFaces;
    
    std::vector<float> edgeVertices;
    std::vector<float> edgeUVs;
    std::vector<float> edgeNormals;
    std::vector<uint32_t> edgeIndices;
    
    std::vector<float> faceVertices;
    std::vector<float> faceUVs;
    std::vector<float> faceEdges;
    std::vector<uint32_t> faceIndices;
    
    std::vector<float> nfaceVertices;
    std::vector<float> nfaceUVs;
    std::vector<uint32_t> nfaceIndices;

    
    bool hola = false;
    
    for(auto mapped: map)
    {
        uint64_t currenHEIndex = mapped.first;
        uint64_t pairIndex = ((currenHEIndex & 0xffffffff) << 32) | (currenHEIndex >> 32);
        HalfEdge* currentHE = mapped.second;
        HalfEdge* pairHE = map[pairIndex];

        if (pairHE && currentHE)
        {
            if(currentHE->pair() == nullptr)
            {
                pairHE->setPair(currentHE);
                currentHE->setPair(pairHE);
                
                uint32_t currentVertexIndex = currentHE->origin();
                uint32_t currentVertexIndexNext = currentHE->next()->origin();
                uint32_t currentVertexIndexNextNext = currentHE->next()->next()->origin();
                glm::vec4 currentUVs{uvBuffer[2 * currentVertexIndex], uvBuffer[2 * currentVertexIndex + 1], uvBuffer[2 * currentVertexIndexNext], uvBuffer[2 * currentVertexIndexNext + 1]};
                
                uint32_t pairVertexIndex = pairHE->origin();
                uint32_t pairVertexIndexNext = pairHE->next()->origin();
                uint32_t pairVertexIndexNextNext = pairHE->next()->next()->origin();
                glm::vec4 pairUVs{uvBuffer[2 * pairVertexIndexNext], uvBuffer[2 * pairVertexIndexNext + 1], uvBuffer[2 * pairVertexIndex], uvBuffer[2 * pairVertexIndex + 1]};
                
                if(currentUVs != pairUVs && !hola)
                {
                    //hola = true;
                    //-LOG("Island border halfEdge: [", currentVertexIndex, ", ", currentVertexIndexNext, "] -> " , glm::to_string(currentUVs));
                    //-LOG("Island border pair halfEdge: [", pairVertexIndex, ", ", pairVertexIndexNext, "] -> opposite of" , glm::to_string(pairUVs));
                    
                    borderHalfEdges.push_back(currentVertexIndex); borderHalfEdges.push_back(currentVertexIndexNext);
                    borderHalfEdges.push_back(pairVertexIndex); borderHalfEdges.push_back(pairVertexIndexNext);
                    
                    const auto& cVertexIndices = currentHE->face()->vertexIndices();
                    const auto& pVertexIndices = pairHE->face()->vertexIndices();

                    std::copy(std::begin(cVertexIndices), std::end(cVertexIndices), std::back_inserter(borderFaces));
                    std::copy(std::begin(pVertexIndices), std::end(pVertexIndices), std::back_inserter(borderFaces));
                    
                    glm::vec2 a(uvBuffer[2 * pairVertexIndexNext], uvBuffer[2 * pairVertexIndexNext + 1]);
                    glm::vec2 b(uvBuffer[2 * pairVertexIndex], uvBuffer[2 * pairVertexIndex + 1]);
                    glm::vec2 c(uvBuffer[2 * pairVertexIndexNextNext], uvBuffer[2 * pairVertexIndexNextNext + 1]);

                    glm::vec2 d(uvBuffer[2 * currentVertexIndex], uvBuffer[2 * currentVertexIndex + 1]);
                    glm::vec2 e(uvBuffer[2 * currentVertexIndexNext], uvBuffer[2 * currentVertexIndexNext + 1]);
                    glm::vec2 g(uvBuffer[2 * currentVertexIndexNextNext], uvBuffer[2 * currentVertexIndexNextNext + 1]);

                    auto edgeNormal = computeNormal(d, e, g);
                    auto edgeNormalPair = computeNormal(a, b, c);                    
                    
                    // --------------- //
                                        
                    edgeVertices.push_back(uvBuffer[2 * currentVertexIndex]);
                    edgeVertices.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    edgeVertices.push_back(0);
                    
                    edgeUVs.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    edgeUVs.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                                                            
                    edgeNormals.push_back(edgeNormal.x);
                    edgeNormals.push_back(edgeNormal.y);
                    edgeNormals.push_back(edgeNormalPair.x);
                    edgeNormals.push_back(edgeNormalPair.y);
                                        
                    edgeIndices.push_back(edgeIndices.size());
                                        
                    edgeVertices.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    edgeVertices.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
                    edgeVertices.push_back(0);

                    edgeUVs.push_back(uvBuffer[2 * pairVertexIndex]);
                    edgeUVs.push_back(uvBuffer[2 * pairVertexIndex + 1]);

                    edgeNormals.push_back(edgeNormal.x);
                    edgeNormals.push_back(edgeNormal.y);
                    edgeNormals.push_back(edgeNormalPair.x);
                    edgeNormals.push_back(edgeNormalPair.y);                    
                                        
                    edgeIndices.push_back(edgeIndices.size());
                                        
                    edgeVertices.push_back(uvBuffer[2 * pairVertexIndex]);
                    edgeVertices.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    edgeVertices.push_back(0);
                    
                    edgeUVs.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    edgeUVs.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
                                        
                    edgeNormals.push_back(edgeNormalPair.x);
                    edgeNormals.push_back(edgeNormalPair.y);
                    edgeNormals.push_back(edgeNormal.x);
                    edgeNormals.push_back(edgeNormal.y);                    
                    
                    edgeIndices.push_back(edgeIndices.size());
                    
                    edgeVertices.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    edgeVertices.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    edgeVertices.push_back(0);

                    edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
                    edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);

                    edgeNormals.push_back(edgeNormalPair.x);
                    edgeNormals.push_back(edgeNormalPair.y);
                    edgeNormals.push_back(edgeNormal.x);
                    edgeNormals.push_back(edgeNormal.y);                    
                    
                    edgeIndices.push_back(edgeIndices.size());

                    // --------------- //
                    
                    faceVertices.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceVertices.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    faceVertices.push_back(0);
                    
                    faceUVs.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    faceUVs.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);

                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);                    
                    
//                     LOG("Texel[",uvBuffer[2 * pairVertexIndexNext],", ", uvBuffer[2 * pairVertexIndexNext + 1], "]"); 
                    
                    faceIndices.push_back(faceIndices.size());
                    
                    faceVertices.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    faceVertices.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
                    faceVertices.push_back(0);

                    faceUVs.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceUVs.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);                    
                    
//                     LOG("Texel[",uvBuffer[2 * pairVertexIndex],", ", uvBuffer[2 * pairVertexIndex + 1], "]"); 
                    
                    faceIndices.push_back(faceIndices.size());

                    auto newPoint = computePoint(a, b, c, d, e, g);

                    faceVertices.push_back(newPoint.x);
                    faceVertices.push_back(newPoint.y);
                    faceVertices.push_back(0);
                    
//                     // -
//                     
//                     edgeVertices.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeVertices.push_back(uvBuffer[2 * currentVertexIndex + 1]);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
// 
//                     edgeVertices.push_back(newPoint.x);
//                     edgeVertices.push_back(newPoint.y);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
//                     
//                     edgeVertices.push_back(newPoint.x);
//                     edgeVertices.push_back(newPoint.y);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
//                     
//                     edgeVertices.push_back(uvBuffer[2 * currentVertexIndexNext]);
//                     edgeVertices.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                      
//                     
//                     // -
                    
                    faceUVs.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceUVs.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);                    
                    
//                     LOG("Texel[",uvBuffer[2 * pairVertexIndexNextNext],", ", uvBuffer[2 * pairVertexIndexNextNext + 1], "]"); 
 
                    faceIndices.push_back(faceIndices.size());

                    
                    faceVertices.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceVertices.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    faceVertices.push_back(0);
                    
                    faceUVs.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    faceUVs.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);

                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex + 1]);                    
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    
                    faceIndices.push_back(faceIndices.size());
                    
                    faceVertices.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    faceVertices.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    faceVertices.push_back(0);

                    faceUVs.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex + 1]);                    
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    
                    faceIndices.push_back(faceIndices.size());

                    newPoint = computePoint(d, e, g, a, b, c);

                    faceVertices.push_back(newPoint.x);
                    faceVertices.push_back(newPoint.y);
                    faceVertices.push_back(0);
                    
                    faceUVs.push_back(uvBuffer[2 * currentVertexIndex]);
                    faceUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndex + 1]);                    
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    faceEdges.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    
                    faceIndices.push_back(faceIndices.size());
                    
//                     // -
//                     
//                     edgeVertices.push_back(uvBuffer[2 * pairVertexIndex]);
//                     edgeVertices.push_back(uvBuffer[2 * pairVertexIndex + 1]);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
// 
//                     edgeVertices.push_back(newPoint.x);
//                     edgeVertices.push_back(newPoint.y);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
//                     
//                     edgeVertices.push_back(newPoint.x);
//                     edgeVertices.push_back(newPoint.y);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                    
//                     
//                     edgeIndices.push_back(edgeIndices.size());
//                     
//                     edgeVertices.push_back(uvBuffer[2 * pairVertexIndexNext]);
//                     edgeVertices.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
//                     edgeVertices.push_back(0);
// 
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex]);
//                     edgeUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
// 
//                     edgeNormals.push_back(edgeNormal.x);
//                     edgeNormals.push_back(edgeNormal.y);
//                     edgeNormals.push_back(0);
//                     edgeNormals.push_back(0);                      
//                     
//                     // -                    
//                     
                    // --------------- // 
                    
                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndex]);
                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    nfaceVertices.push_back(0);
                    
                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    
                    nfaceIndices.push_back(nfaceIndices.size());
                    
                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
                    nfaceVertices.push_back(0);

                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndex]);
                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                                        
                    nfaceIndices.push_back(nfaceIndices.size());

                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndexNextNext]);
                    nfaceVertices.push_back(uvBuffer[2 * currentVertexIndexNextNext + 1]);
                    nfaceVertices.push_back(-100);
                    
                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndex]);
                    nfaceUVs.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    
                    nfaceIndices.push_back(nfaceIndices.size());
                    
                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndex]);
                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndex + 1]);
                    nfaceVertices.push_back(0);
                    
                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndexNext]);
                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndexNext + 1]);
                    
                    nfaceIndices.push_back(nfaceIndices.size());
                    
                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndexNext]);
                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndexNext + 1]);
                    nfaceVertices.push_back(0);

                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndex]);
                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);
                    
                    nfaceIndices.push_back(nfaceIndices.size());

                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndexNextNext]);
                    nfaceVertices.push_back(uvBuffer[2 * pairVertexIndexNextNext + 1]);
                    nfaceVertices.push_back(-100);
                    
                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndex]);
                    nfaceUVs.push_back(uvBuffer[2 * currentVertexIndex + 1]);

                    nfaceIndices.push_back(nfaceIndices.size());                    
                }
            }
        }
        else
        {
            LOG_WARN("Mesh::generateAdjacencyInformation: ", currenHEIndex, " has no pair");            
            noPairEdgeCount++;
        }
    }
    
    updateData(GLBuffer::Vertex, 0, edgeVertices.size() * sizeof(float), edgeVertices.data());
    updateData(GLBuffer::UV, 0, edgeUVs.size() * sizeof(float), edgeUVs.data());
    updateData(GLBuffer::Color, 0, edgeNormals.size() * sizeof(float), edgeNormals.data());
    updateData(GLBuffer::Index, 0, edgeIndices.size() * sizeof(uint32_t), edgeIndices.data());
    
    std::vector<uint32_t> subMeshIndexes(1, static_cast<uint32_t>(edgeIndices.size()));
    updateSubMeshData(subMeshIndexes);

    // ------------------------------ //
    
//     updateData(GLBuffer::Vertex, 0, faceVertices.size() * sizeof(float), faceVertices.data());
//     updateData(GLBuffer::UV, 0, faceUVs.size() * sizeof(float), faceUVs.data());
//     updateData(GLBuffer::Color, 0, faceEdges.size() * sizeof(float), faceEdges.data());
//     updateData(GLBuffer::Index, 0, faceIndices.size() * sizeof(uint32_t), faceIndices.data());
//     
//     std::vector<uint32_t> subMeshIndexes2(1, static_cast<uint32_t>(faceIndices.size()));
//     updateSubMeshData(subMeshIndexes2);
    
    // ------------------------------ //
    
/*    updateData(GLBuffer::Vertex, 0, nfaceVertices.size() * sizeof(float), nfaceVertices.data());
    updateData(GLBuffer::UV, 0, nfaceUVs.size() * sizeof(float), nfaceUVs.data());
    updateData(GLBuffer::Index, 0, nfaceIndices.size() * sizeof(uint32_t), nfaceIndices.data());
    
    std::vector<uint32_t> subMeshIndexes3(1, static_cast<uint32_t>(nfaceIndices.size()));
    updateSubMeshData(subMeshIndexes3);  */  
    
    
    //updateData(GLBuffer::Index2, 0, borderHalfEdges.size() * sizeof(uint32_t), borderHalfEdges.data());
    
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

void Mesh::clearBuffers()
{
    for (std::size_t i = 0; i < mDataBuffers.size(); ++i)
        clearBuffer(static_cast<int>(i));
}


void Mesh::clearBuffer(const int buffer)
{
    if (buffer < static_cast<int>(mDataBuffers[buffer].size()))
    {
        mDataBuffers[buffer].clear();
        mBufferUpdateSize[buffer] = 0;
        mBufferUpdateOffset[buffer] = 0;
        mBufferUpdateSizeState[buffer] = SizeState::Equal;
    }
}

void Mesh::updateData(const int buffer, GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
        
    if((size + offset) > mDataBuffers[buffer].size())
    {
        mDataBuffers[buffer].resize(size + offset);
        mBufferUpdateSizeState[buffer] = SizeState::Bigger;
        for(auto scene : mScenes) (scene.first)->setMeshesDirtyAfter(this);
    }
    else
    {
        mBufferUpdateSizeState[buffer] = SizeState::Equal;
        for(auto scene : mScenes) (scene.first)->setMeshDirty(this);
    }
        
    std::copy(byteData, byteData + size, begin(mDataBuffers[buffer]) + offset);
    
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
    mBufferUpdateSize[buffer] = mDataBuffers[buffer].size();
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


// void Mesh::setVAO()
// {    
//     //LOG("Setting VAO: " << std::endl;
//     //LOG("Vertex: " << mVertexBufferData.size() << std::endl;
//     //LOG("Normals: " << mNormalBufferData.size() << std::endl;
//     //LOG("UVs: " << mUVBufferData.size() << std::endl;
//     //LOG("Index: " << mIndexBufferData.size() << std::endl;
//     
//     
//     glGenVertexArrays(1, &mVAO);
//     glBindVertexArray(mVAO);
//     
// /*    mVertexBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mVertexBufferData.size() * sizeof(GLfloat));
//     mVertexBufferObject->bind();
//     std::copy(mVertexBufferData.begin(), mVertexBufferData.end(), reinterpret_cast<GLfloat *>(mVertexBufferObject->dataPointer()));
//     
//     //memcpy(mVertexBufferObject->dataPointer(), &mVertexBufferData.front(), mVertexBufferData.size() * sizeof(GLfloat));
// 
//     glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(0);
//     
//     
//     mNormalBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mNormalBufferData.size());
//     mNormalBufferObject->bind();
//     std::copy(mNormalBufferData.begin(), mNormalBufferData.end(), reinterpret_cast<GLfloat *>(mNormalBufferObject->dataPointer()));
//     //memcpy(mNormalBufferObject->dataPointer(), &mNormalBufferData.front(), mNormalBufferData.size() * sizeof(GLfloat));
// 
//     glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(1);
// 
//     
//     mUVBufferObject = new PersistentBufferObject(GL_ARRAY_BUFFER, mUVBufferData.size() * sizeof(GLfloat));
//     mUVBufferObject->bind();
//     //memcpy(mUVBufferObject->dataPointer(), &mUVBufferData.front(), mUVBufferData.size() * sizeof(GLfloat));
//     std::copy(mUVBufferData.begin(), mUVBufferData.end(), reinterpret_cast<GLfloat *>(mUVBufferObject->dataPointer()));
// 
//     glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(2);
// 
//     mIndexBufferObject = new PersistentBufferObject(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() * sizeof(GLuint));
//     mIndexBufferObject->bind();
//     //memcpy(mIndexBufferObject->dataPointer(), &mIndexBufferData.front(), mIndexBufferData.size() * sizeof(GLuint));
//     std::copy(mIndexBufferData.begin(), mIndexBufferData.end(), reinterpret_cast<GLuint *>(mIndexBufferObject->dataPointer()));
//     
//     //glGenBuffers(1, &mVBO[3]);
//     //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[3]);
//     //glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() *sizeof(GLuint), &mIndexBufferData.front(), GL_STATIC_DRAW);
//     
//     //LOG("PBO Vertex: " << mVertexBufferObject->id() << ", cap: " << mVertexBufferObject->capacity() << std::endl;
//     //LOG("PBO Normals: " << mNormalBufferObject->id() << ", cap: " << mNormalBufferObject->capacity() << std::endl;
//     //LOG("PBO UVs: " << mUVBufferObject->id() << ", cap: " << mUVBufferObject->capacity() << std::endl;
//     //LOG("PBO Indexes: " << mIndexBufferObject->id() << ", cap: " << mIndexBufferObject->capacity() << std::endl;
//     
//     glBindVertexArray(0);*/
//     
//     /*LOG("VAO: vertices" << std::endl;
//     for(auto vert: mVertexBufferData)
//         LOG(vert << " ";
//     LOG(std::endl;
// 
//     
//     LOG("VAO: indices" << std::endl;
//     for(auto index: mIndexBufferData)
//         LOG(index << " ";
//     LOG(std::endl;*/
//     
//     /*float* vertices = new float[18];
//     float* coordTextura = new float[12];
//   
//     vertices[0] = -1.0; vertices[1] = -1.0; vertices[2] = 0.0; 
//     vertices[3] = -1.0; vertices[4] = 5.0; vertices[5] = 0.0; 
//     vertices[6] = 5.0; vertices[7] = 5.0; vertices[8] = 0.0; 
//   
//     vertices[9] = 5.0; vertices[10] = -1.0; vertices[11] = 0.0; 
//     vertices[12] = -1.0; vertices[13] = -1.0; vertices[14] = 0.0; 
//     vertices[15] = 5.0; vertices[16] = 5.0; vertices[17] = 0.0; 
//     
//     coordTextura[0] = 0.0; coordTextura[1] = 0.0;
//     coordTextura[2] = 0.0; coordTextura[3] = 1.0;
//     coordTextura[4] = 1.0; coordTextura[5] = 1.0;
// 
//     coordTextura[6] = 1.0; coordTextura[7] = 0.0;
//     coordTextura[8] = 0.0; coordTextura[9] = 0.0;
//     coordTextura[10] = 1.0; coordTextura[11] = 1.0;
//     
//     GLuint indices[6] = {0,1,2,3,4,5};
//     */
//     
//     
// /*    glGenBuffers(1, &mVBO[0]);
//     glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
//     glBufferData(GL_ARRAY_BUFFER, mVertexBufferData.size() *sizeof(float), &mVertexBufferData.front(), GL_STATIC_DRAW);
//     //glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
//     
//     glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(0);
// 
//     glGenBuffers(1, &mVBO[1]);
//     glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
//     glBufferData(GL_ARRAY_BUFFER, mNormalBufferData.size() *sizeof(float), &mNormalBufferData.front(), GL_STATIC_DRAW);
//     
//     glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(1);
//     
//     glGenBuffers(1, &mVBO[2]);
//     glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
//     glBufferData(GL_ARRAY_BUFFER, mUVBufferData.size() *sizeof(float), &mUVBufferData.front(), GL_STATIC_DRAW);
//     
//     glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
//     
//     glEnableVertexAttribArray(2);
//     
//     glGenBuffers(1, &mVBO[3]);
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO[3]);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferData.size() *sizeof(GLuint), &mIndexBufferData.front(), GL_STATIC_DRAW);
//     
//     glBindVertexArray(0);
//     
//     LOG("VBO vertices size: ", mVBO[0]);
//     LOG("VBO Normals size: ", mVBO[1]);
//     LOG("VBO UVs size: ", mVBO[2]);
//     LOG("VBO Indexes size: ", mVBO[3]);*/
// }



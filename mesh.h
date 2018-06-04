#ifndef MESH_H
#define MESH_H


#include "persistentbufferobject.h"
#include "const.h"
#include "cerealglm.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include <cereal/types/string.hpp>


class Vertex;
class Face;
class Edge;
class Scene3D;

class HalfEdge
{
public:
    HalfEdge();

    HalfEdge* next(){ return mNext; }
    HalfEdge* previous(){ return mPrevious; }
    HalfEdge* pair(){ return mPair; }
    Vertex* origin(){ return mOrigin; }
    Face* face(){ return mLeft; }
    
    void setNext(HalfEdge* next){ mNext = next;}
    void setPrevious(HalfEdge* previous){ mPrevious = previous; }
    void setPair(HalfEdge* pair){ mPair = pair; }
    void setOrigin(Vertex* origin){ mOrigin = origin; }
    void setFace(Face* face){ mLeft = face; }
    
private:
    HalfEdge* mNext;
    HalfEdge* mPrevious;
    HalfEdge* mPair;

    Vertex* mOrigin;
    Face* mLeft;
};

class Vertex
{
public:
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord);
    glm::vec3 position() const { return mPosition; }
    glm::vec3 normal() const { return mNormal; }
    glm::vec2 textureCoord() const { return mTextureCoord; }
    glm::vec4 color() const { return mColor; }

private:
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec2 mTextureCoord;
    glm::vec4 mColor;
};

class Edge
{
};

class Face
{
public:
    Face(uint32_t index1, uint32_t index2, uint32_t index3, HalfEdge* he = nullptr);
private:
    glm::uvec3 mIndices;
    HalfEdge* mHE;
};

struct BoundingBox
{
    glm::vec3 max;
    glm::vec3 min;
    glm::vec3 center;
    
    float width() const { return fabs(max.x - min.x); }
    float height() const { return fabs(max.y - min.y); }
    float depth() const { return fabs(max.z - min.z); }
    
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(max, min, center);
    }
};

struct SubMesh
{
  SubMesh() : IndexOffset(0), TriangleCount(0) {}

  unsigned int IndexOffset;
  unsigned int TriangleCount;
};

class Mesh
{
public:
    
    enum class SizeState : unsigned char
    {
        Smaller = 0,
        Equal = 1,
        Bigger = 2
    };
    
    Mesh() : Mesh("") {};
    Mesh(std::string name);
    ~Mesh();
        
    const std::string& name() const { return mName; }

    GLuint VAO() const { return mVAO; }
    
    const std::vector<GLuint>& subMeshes() const { return mSubMeshes; }    
    const std::vector<Vertex *>& vertices() const { return mVertices; }
    const std::vector<Face *>& faces() const { return mFaces; }
    
    std::size_t count(const int dataType) const;
    std::size_t verticesCount() const { return mVertices.size(); }
    std::size_t vertexCount() const { return mBufferData[GLBuffer::Vertex].size()/(3 * sizeof(GLfloat)); }
    std::size_t elementCount() const { return mBufferData[GLBuffer::Index].size()/sizeof(GLuint); }
    
    const std::vector<glm::byte>& buffer(const int buffer) const { return mBufferData[buffer]; }    
    const std::vector<glm::byte>& vertexBuffer() const { return mBufferData[GLBuffer::Vertex]; }
    const std::vector<glm::byte>& normalBuffer() const { return mBufferData[GLBuffer::Normal]; }
    const std::vector<glm::byte>& uvBuffer() const { return mBufferData[GLBuffer::UV]; }
    const std::vector<glm::byte>& colorBuffer() const { return mBufferData[GLBuffer::Color]; }    
    const std::vector<glm::byte>& indexBuffer() const { return mBufferData[GLBuffer::Index]; }
    
    const std::unordered_set<unsigned int>& updatedBuffers() const { return mUpdatedBuffers; }
    bool bufferNeedUpdate(const int buffer) const { return mUpdatedBuffers.find(buffer) != mUpdatedBuffers.end(); }
    bool vertexBufferNeedUpdate() const { return mUpdatedBuffers.find(GLBuffer::Vertex) != mUpdatedBuffers.end(); }
    bool normalBufferNeedUpdate() const { return mUpdatedBuffers.find(GLBuffer::Normal) != mUpdatedBuffers.end(); }
    bool uvBufferNeedUpdate() const { return mUpdatedBuffers.find(GLBuffer::UV) != mUpdatedBuffers.end(); }    
    bool indexBufferNeedUpdate() const { return mUpdatedBuffers.find(GLBuffer::Index) != mUpdatedBuffers.end(); }    
    
    std::size_t bufferUpdateSize(const int buffer) const { return mBufferUpdateSize[buffer]; }
    std::size_t vertexBufferUpdateSize() const { return mBufferUpdateSize[GLBuffer::Vertex]; }
    std::size_t normalBufferUpdateOffset() const { return mBufferUpdateOffset[GLBuffer::Normal]; }
    std::size_t uvBufferUpdateSize() const { return mBufferUpdateSize[GLBuffer::UV]; }
    std::size_t indexBufferUpdateSize() const { return mBufferUpdateSize[GLBuffer::Index]; }
    
    std::size_t bufferUpdateOffset(const int buffer) const { return mBufferUpdateOffset[buffer]; }
    std::size_t vertexBufferUpdateOffset() const { return mBufferUpdateOffset[GLBuffer::Vertex]; }
    std::size_t normalBufferUpdateSize() const { return mBufferUpdateSize[GLBuffer::Normal]; }
    std::size_t uvBufferUpdateOffset() const { return mBufferUpdateOffset[GLBuffer::UV]; }
    std::size_t indexBufferUpdateOffset() const { return mBufferUpdateOffset[GLBuffer::Index]; }
    
    SizeState bufferUpdateSizeState(const int buffer) const { return mBufferUpdateSizeState[buffer]; }
    bool indexBuffer2Used() const { return mUseIndexBuffer2; }

    const BoundingBox& boundingBox() const { return mBoundingBox; }
    glm::vec3 dimensions() const { return glm::vec3(mBoundingBox.width(), mBoundingBox.height(), mBoundingBox.depth()); }    
    float width() const { return mBoundingBox.width(); }
    float height() const { return mBoundingBox.height(); }
    float depth() const { return mBoundingBox.depth(); }    

    int sceneIndex(Scene3D* scene) const;
    
    void setName(std::string name) { mName = name; }
    void setVAO();
            
    void updateSubMeshData(const std::vector<GLuint>& newData, int offset = 0);
    void insertVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoord);
    void insertFace(uint32_t index1, uint32_t index2, uint32_t index3);
    void generateAdjacencyInformation();    

    void clearUpdatedBuffers();
    void clearBuffers();
    void clearBuffer(const int buffer);
    void updateData(const int buffer, GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void updateVertexData(const std::vector<GLfloat>& newData, int offset = 0);
    void updateNormalData(const std::vector< GLfloat >& newData, int offset = 0);
    void updateUVData(const std::vector<GLfloat>& newData, int offset = 0);
    void updateIndexData(const std::vector<GLuint>& newData, int offset = 0);
    
    void setBufferNeedUpdate(const int buffer, bool update){ if(update) mUpdatedBuffers.insert(buffer); else mUpdatedBuffers.erase(buffer); }
    void setVertexBufferNeedUpdate(bool update){ setBufferNeedUpdate(GLBuffer::Vertex, update); }
    void setNormalBufferNeedUpdate(bool update){ setBufferNeedUpdate(GLBuffer::Normal, update); }
    void setUVBufferNeedUpdate(bool update){ setBufferNeedUpdate(GLBuffer::UV, update); }
    void setIndexBufferNeedUpdate(bool update){ setBufferNeedUpdate(GLBuffer::Index, update); }
    
    void setBufferDirty(const int buffer);
    void useIndexBuffer2(bool use);
    
    bool addScene(Scene3D *scene, int index);
    bool removeScene(Scene3D *scene);
    
    void setBoundingBox(glm::vec3 min, glm::vec3 max);
    
    // temp fix
    bool resizeBufferCount();

    template<class Archive>
    void serialize(Archive& archive)
    {
        archive(mName, mSubMeshes, mBoundingBox, mBufferData);
    }
    
protected:
    std::string mName;

    std::vector<GLuint> mSubMeshes;    
    std::vector<Vertex *> mVertices;
    std::vector<Face *> mFaces;
    std::vector<Edge *> mEdges;
    std::vector<HalfEdge *> mHalfEdges;
    std::unordered_map<unsigned long, HalfEdge *> map;
    
    GLuint mVAO;
    GLuint mVBO[4];
    PersistentBufferObject *mVertexBufferObject;
    PersistentBufferObject *mNormalBufferObject;
    PersistentBufferObject *mUVBufferObject;
    PersistentBufferObject *mIndexBufferObject;
    
    std::vector<std::vector<glm::byte>> mBufferData;
    std::vector<std::size_t> mBufferUpdateSize;
    std::vector<std::size_t> mBufferUpdateOffset;
    std::vector<SizeState> mBufferUpdateSizeState;     
    std::unordered_set<unsigned int> mUpdatedBuffers;
    bool mUseIndexBuffer2 = false;
    
    std::vector<std::pair<Scene3D*, int>> mScenes;
    
    BoundingBox mBoundingBox;
};

#endif // MESH_H

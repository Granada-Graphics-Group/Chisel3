#include "circle.h"
#include "logger.hpp"

Circle::Circle(std::string name, glm::vec3 center, float radius, int slices)
:
    Mesh(name),
    mCenter(center),
    mRadius(radius),
    mSlices(slices)
{
    changeDimensions(mRadius, mSlices);
}

// *** Public Methods *** //

void Circle::changeDimensions(float radius, int slices)
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texCoords;
    std::vector<GLuint> indices;
    
    mCenter += glm::vec3(radius - mRadius, radius - mRadius, 0);
    mRadius = radius;
    mSlices = slices;
    
    glm::vec4 color(0.5, 0.5, 0.5, 1.0);
    glm::vec3 normal(0, 0, 1);
    GLfloat delta = 2.0f * glm::pi<GLfloat>() / mSlices;
    
    vertices.insert(end(vertices), begin(mCenter), end(mCenter));
    colors.insert(end(colors), begin(color), end(color));    
    normals.insert(end(normals), begin(normal), end(normal));
        
    texCoords.push_back(0.5f);
    texCoords.push_back(0.5f);
    
    for(int i = 0; i < mSlices; i++)
    {
        LOG("index: ", i, " x: ", mCenter.x + (mRadius * glm::cos(i * delta)), " y: ", mCenter.y + (mRadius * glm::sin(i * delta)));
        
        vertices.push_back(mCenter.x + (mRadius * glm::cos(i * delta)));
        vertices.push_back(mCenter.y + (mRadius * glm::sin(i * delta)));
        vertices.push_back(mCenter.z);

        colors.insert(end(colors), begin(color), end(color));
        
        normals.insert(end(normals), begin(normal), end(normal));

        LOG("Tx: ", (glm::cos(i * delta) + 1.0f) * 0.5f, " Ty: ", (glm::sin(i * delta) + 1.0f) * 0.5f);
        
        texCoords.push_back((glm::cos(i * delta) + 1.0f) * 0.5f);
        texCoords.push_back((glm::sin(i * delta) + 1.0f) * 0.5f);
    }
    
    for(int i = 0; i < (mSlices - 1); i++)
    {
        indices.push_back(0);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
    }

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(static_cast<GLuint>(vertices.size()/3) - 1);
    
    std::vector<GLuint> subMeshIndexes = { static_cast<GLuint>(indices.size()) };
    
    updateData(GLBuffer::Vertex, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());    
    updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    updateData(GLBuffer::UV, 0, texCoords.size() * sizeof(GLfloat), texCoords.data());
    updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());
    updateSubMeshData(subMeshIndexes);
    
    setBoundingBox(mCenter + glm::vec3(-mRadius, - mRadius, 0.f), mCenter + glm::vec3(mRadius, mRadius, 0.f));
}

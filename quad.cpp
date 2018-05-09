#include "quad.h"

Quad::Quad(std::string name, glm::vec2 orig, glm::vec2 dimensions, glm::vec4 color)
:
    Mesh(name)
{
    std::vector<GLfloat> vertex =
    {
        orig.x + dimensions.x, orig.y + dimensions.y, 0.0,
        orig.x, orig.y + dimensions.y, 0.0,
        orig.x, orig.y, 0.0,
        orig.x + dimensions.x, orig.y, 0.0
    };

    std::vector<GLfloat> colors =
    {
        color.r, color.g, color.b, color.a,
        color.r, color.g, color.b, color.a,
        color.r, color.g, color.b, color.a,
        color.r, color.g, color.b, color.a
    };

    std::vector<GLfloat> normals =
    {
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0
    };

    std::vector<GLfloat> coordTex =
    {
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0
    };

    std::vector<GLuint> indices = { 0, 1, 2, 0, 2, 3 };
    std::vector<GLuint> subMeshIndexes = { 6 };

    updateData(GLBuffer::Vertex, 0, vertex.size() * sizeof(GLfloat), vertex.data());
    updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());
    updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    updateData(GLBuffer::UV, 0, coordTex.size() * sizeof(GLfloat), coordTex.data());
    updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());

    updateSubMeshData(subMeshIndexes);
    setBoundingBox(glm::vec3(orig, 0.0), glm::vec3(orig.x + dimensions.x, orig.y + dimensions.y, 0.0));
}


// *** Public Methods *** //

void Quad::changeDimensions(glm::vec2 dimensions)
{
    resize(mBoundingBox.min, dimensions);
}

void Quad::resize(glm::vec2 orig, glm::vec2 dimensions)
{
    std::vector<GLfloat> vertex =
    {
        orig.x + dimensions.x, orig.y + dimensions.y, 0.0,
        orig.x, orig.y + dimensions.y, 0.0,
        orig.x, orig.y, 0.0,
        orig.x + dimensions.x, orig.y, 0.0
    };
        
    updateData(GLBuffer::Vertex, 0, vertex.size() * sizeof(GLfloat), vertex.data());
    
    setBoundingBox(glm::vec3(orig, 0.0), glm::vec3(orig.x + dimensions.x, orig.y + dimensions.y, 0.0));    
}

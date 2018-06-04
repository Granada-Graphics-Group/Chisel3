#include "disk.h"
#include "logger.hpp"

Disk::Disk(std::string name, float innerRadius, float outerRadius, int slices)
:
    Mesh(name),
    mCenter(glm::vec3(outerRadius, outerRadius, 0)),
    mInnerRadius(innerRadius),
    mOuterRadius(outerRadius),
    mSlices(slices)
{
    changeDimensions(mInnerRadius, mOuterRadius, mSlices);
}

Disk::~Disk()
{

}


// *** Public Methods *** //

void Disk::changeDimensions(float innerRadius, float outerRadius, int slices)
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texCoords;
    std::vector<GLuint> indices;
    
    mCenter = glm::vec3(outerRadius, outerRadius, 0);
    mInnerRadius = innerRadius;
    mOuterRadius = outerRadius;
    mSlices = slices;

    glm::vec4 color(0.5, 0.5, 0.5, 1.0);    
    glm::vec3 normal(0, 0, 1);

    GLfloat deltaAngle = 2.0f * glm::pi<GLfloat>() / mSlices;
    GLfloat deltaRadius = (outerRadius - innerRadius) / outerRadius;
    
    for(int i = 0; i < mSlices; i++)
    {
        LOG("index: ", i, " width: ", outerRadius - innerRadius, " x: ", mCenter.x + (mOuterRadius * glm::cos(i * deltaAngle)), " y: ", mCenter.y + (mOuterRadius * glm::sin(i * deltaAngle)));
        LOG("index: ", i + 1, " width: ", outerRadius - innerRadius, " x: ", mCenter.x + (mInnerRadius * glm::cos(i * deltaAngle)), " y: ", mCenter.y + (mInnerRadius * glm::sin(i * deltaAngle)));
        
        vertices.push_back(mCenter.x + (mOuterRadius * glm::cos(i * deltaAngle)));
        vertices.push_back(mCenter.y + (mOuterRadius * glm::sin(i * deltaAngle)));
        vertices.push_back(mCenter.z);
        
        vertices.push_back(mCenter.x + (mInnerRadius * glm::cos(i * deltaAngle)));
        vertices.push_back(mCenter.y + (mInnerRadius * glm::sin(i * deltaAngle)));
        vertices.push_back(mCenter.z);

        colors.insert(end(colors), begin(color), end(color));
        
        colors.insert(end(colors), begin(color), end(color));        
        
        normals.insert(end(normals), begin(normal), end(normal));
        
        normals.insert(end(normals), begin(normal), end(normal));       

        LOG("Tx: ", (glm::cos(i * deltaAngle) + 1.0f) * 0.5f, " Ty: ", (glm::sin(i * deltaAngle) + 1.0f) * 0.5f);
        
        texCoords.push_back((glm::cos(i * deltaAngle) + 1.0f) * 0.5f);
        texCoords.push_back((glm::sin(i * deltaAngle) + 1.0f) * 0.5f);
        
        texCoords.push_back(((1.0 - deltaRadius) * glm::cos(i * deltaAngle) + 1.0f) * 0.5f);
        texCoords.push_back(((1.0 - deltaRadius) * glm::sin(i * deltaAngle) + 1.0f) * 0.5f);        
    }
    
    for(int i = 0; i < (mSlices - 1); i++)
    {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 3);
        indices.push_back(2 * i + 1);

        
        indices.push_back(2 * i);
        indices.push_back(2 * i + 2);
        indices.push_back(2 * i + 3);
    }

    indices.push_back(static_cast<GLuint>(vertices.size()/3) - 2);
    indices.push_back(1);
    indices.push_back(static_cast<GLuint>(vertices.size()/3) - 1);
    
    indices.push_back(static_cast<GLuint>(vertices.size()/3) - 2);
    indices.push_back(0);
    indices.push_back(1);


    
    std::vector<GLuint> subMeshIndexes = { static_cast<GLuint>(indices.size()) };
    
    updateData(GLBuffer::Vertex, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    updateData(GLBuffer::UV, 0, texCoords.size() * sizeof(GLfloat), texCoords.data());
    updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());
    updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());
    updateSubMeshData(subMeshIndexes);
    
    setBoundingBox(mCenter + glm::vec3(-mOuterRadius, - mOuterRadius, 0.f), mCenter + glm::vec3(mOuterRadius, mOuterRadius, 0.f));
}

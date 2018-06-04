#include "cylinder.h"

Cylinder::Cylinder(std::string name, float bottomRadius, float topRadius, float height, uint32_t radialSlices, uint32_t heightSteps, bool isOpen, glm::vec4 color)
:
    Mesh(name),
    mBottomRadius(bottomRadius),
    mTopRadius(topRadius),
    mHeight(height),
    mRadialSlices(radialSlices),
    mHeightSteps(heightSteps),
    mOpeness(isOpen),
    mColor(color)
{
    resize(mBottomRadius, mTopRadius, mHeight);
}


// *** Public Methods *** //

void Cylinder::resize(float bottomRadius, float topRadius, float height)
{    
    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<uint32_t> indices;
    
    mBottomRadius = bottomRadius;
    mTopRadius = topRadius;
    mHeight = height;
    
    float u, v;    
    float halfHeight = mHeight/2.0;
    float slope = (mBottomRadius - mTopRadius) / mHeight;
    uint32_t index = 0;
    
    // torso
    
    std::vector<std::vector<uint32_t>> vecIndices;
    
    for(auto step = 0; step <= mHeightSteps; ++step)
    {
        std::vector<uint32_t> rowIndices;
        
        v = step / static_cast<float>(mHeightSteps);        
        auto rowRadius = v * (mBottomRadius - mTopRadius) + mTopRadius;
        
        for(auto slice = 0; slice <= mRadialSlices; ++slice)
        {
            u = slice / static_cast<float>(mRadialSlices);
            
            auto tetha = u * 2.0f * glm::pi<float>();
            
            auto tethaSin = glm::sin(tetha);
            auto tethaCos = glm::cos(tetha);
            
            vertices.push_back(rowRadius * tethaSin);
            vertices.push_back(-v * mHeight + halfHeight);
            vertices.push_back(rowRadius * tethaCos);
            
            glm::vec3 normal(tethaSin, slope, tethaCos);
            normal = glm::normalize(normal);
            
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
            
            colors.push_back(mColor.r);
            colors.push_back(mColor.g);
            colors.push_back(mColor.b);
            colors.push_back(mColor.a);
            
            texCoords.push_back(u);
            texCoords.push_back(1 - v);
            
            rowIndices.push_back(index++);
        }
        
        vecIndices.push_back(rowIndices);
    }
    
    for(auto slice = 0; slice < mRadialSlices; ++slice)
        for(auto step = 0; step < mHeightSteps; ++step)
        {
            auto a = vecIndices[step][slice];
            auto b = vecIndices[step + 1][slice];
            auto c = vecIndices[step + 1][slice + 1];
            auto d = vecIndices[step][slice + 1];
            
            indices.push_back(a); indices.push_back(b); indices.push_back(d);
            indices.push_back(b); indices.push_back(c); indices.push_back(d);
        }
    
    clearBuffers();

    updateData(GLBuffer::Vertex, 0, vertices.size() * sizeof(GLfloat), vertices.data());
    updateData(GLBuffer::Color, 0, colors.size() * sizeof(GLfloat), colors.data());    
    updateData(GLBuffer::Normal, 0, normals.size() * sizeof(GLfloat), normals.data());
    updateData(GLBuffer::UV, 0, texCoords.size() * sizeof(GLfloat), texCoords.data());
    updateData(GLBuffer::Index, 0, indices.size() * sizeof(GLuint), indices.data());
    
    if(!mOpeness)
    {
        generateCap(true, buffer(GLBuffer::Vertex).size() / (3 * sizeof(float)));
        generateCap(false, buffer(GLBuffer::Vertex).size() / (3 * sizeof(float)));
    }
    
    std::vector<GLuint> subMeshIndexes = { static_cast<GLuint>(buffer(GLBuffer::Index).size() / sizeof(uint32_t)) };
        
    updateSubMeshData(subMeshIndexes);     
}


// *** Private Methods *** //

void Cylinder::generateCap(bool isTop, uint32_t initIndex)
{
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> colors;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> texCoords;
    std::vector<GLuint> indices;
    float halfHeight = mHeight/2.0;
    
    float radius;
    char sign;
    
    if(isTop)
    {
        radius = mTopRadius;
        sign = 1;        
    }
    else
    {
        radius = mBottomRadius;
        sign = -1;
    }

    auto center = glm::vec3(0, sign * halfHeight, 0);
    glm::vec3 normal(0, sign, 0);
    GLfloat delta = 2.0f * glm::pi<GLfloat>() / mRadialSlices;
    
    vertices.insert(end(vertices), begin(center), end(center));
    colors.insert(end(colors), begin(mColor), end(mColor));    
    normals.insert(end(normals), begin(normal), end(normal));
        
    texCoords.push_back(0.5f);
    texCoords.push_back(0.5f);
    
    for(int i = 0; i < mRadialSlices; i++)
    {
        //LOG("index: ", i, " x: ", mCenter.x + (mRadius * glm::cos(i * delta)), " y: ", mCenter.y + (mRadius * glm::sin(i * delta)));
        
        vertices.push_back(center.x + (radius * glm::sin(i * delta)));
        vertices.push_back(center.y);
        vertices.push_back(center.z + (radius * glm::cos(i * delta)));

        colors.insert(end(colors), begin(mColor), end(mColor));
        
        normals.insert(end(normals), begin(normal), end(normal));

        //LOG("Tx: ", (glm::cos(i * delta) + 1.0f) * 0.5f, " Ty: ", (glm::sin(i * delta) + 1.0f) * 0.5f);
        
        texCoords.push_back((glm::cos(i * delta) + 1.0f) * 0.5f);
        texCoords.push_back((glm::sin(i * delta) + 1.0f) * 0.5f);
    }
    
    for(int i = 0; i < (mRadialSlices - 1); i++)
    {
        indices.push_back(initIndex);
        indices.push_back(initIndex + i + 2);
        indices.push_back(initIndex  + i + 1);
    }

    indices.push_back(initIndex);
    indices.push_back(initIndex + 1);
    indices.push_back(static_cast<GLuint>(initIndex + vertices.size()/3) - 1);
    
    updateData(GLBuffer::Vertex, buffer(GLBuffer::Vertex).size(), vertices.size() * sizeof(GLfloat), vertices.data());
    updateData(GLBuffer::Color, buffer(GLBuffer::Color).size(), colors.size() * sizeof(GLfloat), colors.data());    
    updateData(GLBuffer::Normal, buffer(GLBuffer::Normal).size(), normals.size() * sizeof(GLfloat), normals.data());
    updateData(GLBuffer::UV, buffer(GLBuffer::UV).size(), texCoords.size() * sizeof(GLfloat), texCoords.data());
    updateData(GLBuffer::Index, buffer(GLBuffer::Index).size(), indices.size() * sizeof(GLuint), indices.data());   
}


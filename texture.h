/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Loki <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/raw_data.hpp>
#include <vector>
#include <string>

enum class SamplerType : unsigned int
{
    Signed,
    Unsigned,
    Float
};

class TextureArray;

class Texture
{
public:
    Texture(){};
    Texture(std::string name, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const std::vector<glm::byte>& data, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, bool generate);

    ~Texture();
    
    std::string name() const { return mName; }
    GLuint id() const { return mId; }
    GLenum target() const { return mTarget; }
    GLenum minFilter() const { return mMinFilter; }
    GLenum magFilter() const { return mMagFilter; }
    
    bool isUsingAnisotropicFiltering(){ return mAFLevel > 0; }
    GLfloat anisoFiltLevel() const { return mAFLevel; }
    
    GLint level() const { return mLevel; }
    GLint internalFormat() const { return mInternalFormat; }
    GLsizei width() const { return mWidth; }
    GLsizei height() const { return mHeight; }
    glm::ivec2 dimensions() const { return glm::ivec2(mWidth, mHeight); }    
    GLenum format() const { return mFormat; }
    GLenum type() const { return mType; }
    SamplerType samplerType() const { return mSamplerType; }
    const glm::byte* data() const { return (mData.size() > 0) ? mData.data() : nullptr; }
    std::vector<glm::byte> textureData() const;
    bool generated() const { return mGenerated; }
    TextureArray* textureArray() const { return mArray; }
    
    GLuint textureArrayId() const { return mArrayId; }
    unsigned int textureArrayIndex() const { return mArrayIndices.x; }
    unsigned int textureArrayLayerIndex() const { return mArrayIndices.y; }
    glm::uvec2 textureArrayIndices() const { return mArrayIndices; }
    
    void setName(const std::string& name) { mName = name; }
    void setId(GLuint id){ mId = id; }
    void setTarget(GLenum target){ mTarget = target; }
    void setMinFilter(GLenum filter){ mMinFilter = filter; }
    void setMagFilter(GLenum filter){ mMagFilter = filter; }
    void setAFLevel(GLfloat level){ mAFLevel = level; }
    
    void setLevel(GLint level){ mLevel = level; }
    void setInternalFormat(GLint intForm){ mInternalFormat = intForm; } 
    void setWidth(GLsizei width){ mWidth = width; }
    void setHeight(GLsizei height){ mHeight = height; }
    void setFormat(GLenum format){ mFormat = format; }
    void setType(GLenum type){ mType = type; }
    void setSamplerType(SamplerType type){ mSamplerType = type; }
    void setData(const std::vector<glm::byte>& data) { mData = data; }
    
    void setArray(unsigned int id, unsigned int array, unsigned int layer) { mArrayId = id; mArrayIndices = {array, layer}; }
    void setTextureArray(TextureArray* array) { mArray = array; }

private:    
    std::string mName;
    GLuint mId;    
    GLenum mTarget; 
    GLint mLevel;
    GLint mInternalFormat;
    GLsizei mWidth;
    GLsizei mHeight;
    GLenum mFormat;
    GLenum mType;
    std::vector<glm::byte> mData;
    GLenum mMinFilter;
    GLenum mMagFilter;    
    GLfloat mAFLevel;
    SamplerType mSamplerType;


    GLuint mArrayId = 0;
    glm::uvec2 mArrayIndices;
    TextureArray* mArray;
    
    bool mGenerated;
};

#endif // TEXTURE_H


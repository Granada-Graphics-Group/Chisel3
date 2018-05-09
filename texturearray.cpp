#include "texturearray.h"
#include "glutils.h"
#include "logger.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <numeric>
#include <iostream>
#include <array>


TextureArray::TextureArray(unsigned int index, unsigned int layerCount, std::vector< Texture* > textures, GLenum target, GLint level, GLenum internalFormat, int width, int height, GLenum format, GLenum type, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, GLboolean sparseness)
:
    mIndex(index),
    mLayerCount(layerCount),    
    mTextures(layerCount),
    mFreeList(mLayerCount - textures.size()),
    mTarget(target),
    mLevel(level),
    mInternalFormat(internalFormat),
    mWidth(width),
    mHeight(height),
    mFormat(format),
    mType(type),
    mMinFilter(minFilter),
    mMagFilter(magFilter),
    mAFLevel(anitropyLevel),
    mSparseness(sparseness)
{
    std::list<Texture *> ordered;
    
    for(auto texture: textures)
        (texture->data() != nullptr) ? ordered.push_back(texture) : ordered.push_front(texture);
    
    std::copy(std::make_move_iterator(std::begin(ordered)), std::make_move_iterator(std::end(ordered)), std::begin(mTextures));
    std::iota(rbegin(mFreeList), rend(mFreeList), textures.size());
    
    GLint pageSizeCount;
    std::array<GLint, 10> pageSizeX;
    std::array<GLint, 10> pageSizeY;
    std::array<GLint, 10> pageSizeZ;
    
    glGetInternalformativ(mTarget, mInternalFormat, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, sizeof(GLint), &pageSizeCount);
    glGetInternalformativ(mTarget, mInternalFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, pageSizeCount * sizeof(GLint), pageSizeX.data());
    glGetInternalformativ(mTarget, mInternalFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, pageSizeCount * sizeof(GLint), pageSizeY.data());
    glGetInternalformativ(mTarget, mInternalFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, pageSizeCount * sizeof(GLint), pageSizeZ.data());
    
    LOG("InternalFormat: ", mInternalFormat, ", Page Size Count: ", pageSizeCount);
    std::string sizes;
    for(int i = 0; i < pageSizeCount; ++i) sizes += std::to_string(pageSizeX[i]) + " "; 
    LOG("- X: ", sizes); sizes.clear();
    for(int i = 0; i < pageSizeCount; ++i) sizes += std::to_string(pageSizeY[i]) + " "; 
    LOG("- Y: ", sizes); sizes.clear();
    for(int i = 0; i < pageSizeCount; ++i) sizes += std::to_string(pageSizeZ[i]) + " "; 
    LOG("- Z: ", sizes);

    
    glCreateTextures(mTarget, 1, &mId);
    
    //glTextureParameteri(mId, GL_TEXTURE_SPARSE_ARB, mSparseness);
    
    if(mTarget == GL_TEXTURE_2D_ARRAY)
    {
        //Allocate the storage.
        glTextureParameteri(mId, GL_TEXTURE_SPARSE_ARB, mSparseness);
        glTextureStorage3D(mId, 1, mInternalFormat, mWidth, mHeight, layerCount);
        
        //Upload pixel data.
        //The first 0 refers to the mipmap level (level 0, since there's only 1)
        //The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
        //The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
        //Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.

        for(unsigned int i = 0; i < textures.size(); ++i)
        {
            if(mSparseness)
                glTexturePageCommitmentEXT(mId, 0, 0, 0, i, mWidth, mHeight, 1, GL_TRUE);
            Texture *texture = mTextures[i];
            
            if(texture->data() != nullptr)
                glTextureSubImage3D(mId, 0, 0, 0, i, mWidth, mHeight, 1, mFormat, mType, texture->data());
            else
                mCleaningList.push_back(i);
                                    
            mTextures[i]->setArray(mId, mIndex, i);
            mTextures[i]->setTextureArray(this);
        }
    }
    else if(mTarget == GL_TEXTURE_1D_ARRAY)
    {
        glTextureStorage2D(mId, 1, mInternalFormat, mWidth, layerCount);
       
        for(unsigned int i = 0; i < textures.size(); ++i)
        {
            //glTexturePageCommitmentEXT(mId, 0, 0, i, 0, mWidth, mHeight, 1, GL_TRUE);
            Texture *texture = mTextures[i];

            if(texture->data() != nullptr)
                glTextureSubImage2D(mId, 0, 0, i, mWidth, 1, mFormat, mType, texture->data());
            else
                mCleaningList.push_back(i);
            
            mTextures[i]->setArray(mId, mIndex, i);
            mTextures[i]->setTextureArray(this);
        }
    }
    
    //Always set reasonable texture parameters
    glTextureParameteri(mId, GL_TEXTURE_MIN_FILTER, mMinFilter);
    glTextureParameteri(mId, GL_TEXTURE_MAG_FILTER, mMagFilter);
    glTextureParameteri(mId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(mId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    if(mAFLevel > 0)
        glTextureParameteri(mId, GL_TEXTURE_MAX_ANISOTROPY_EXT, mAFLevel);   

    glGenerateTextureMipmap(mId);
}

TextureArray::~TextureArray()
{
    LOG("Destroying Texture array: ", mId);
	for (int i = 0; i < mLayerCount; ++i)
		freeLayer(i);
    glDeleteTextures(1, &mId);
}

// *** Public Methods *** //
void TextureArray::updateTexture(Texture* texture, unsigned int index)
{
    LOG("Updating texture in TextureArray[", texture->textureArrayIndex(), "]: ", mId, " index: ", index, " with ", texture->name());
    
    mTextures[index] = texture;
    
//     glBindTexture(mTarget, mId);
    
    glTextureSubImage3D(mId, 0, 0, 0, index, mWidth, mHeight, 1, mFormat, mType, texture->data());    
    glGenerateTextureMipmap(mId);
}


void TextureArray::updateTextureData(unsigned int index)
{
    Texture *texture = mTextures[index];
    LOG("Updating texture ", texture->name(), " data in TextureArray[", texture->textureArrayIndex(), "]: ", mId, " index: ", index);
    
//     glBindTexture(mTarget, mId);
    
    glTextureSubImage3D(mId, 0, 0, 0, index, mWidth, mHeight, 1, mFormat, mType, texture->data());    
    glGenerateTextureMipmap(mId);
}

Texture* TextureArray::texture(std::string texture) const
{
    for(auto tex: mTextures)
        if(tex != nullptr && !texture.compare(tex->name()))
            return tex;
    return nullptr;
}

std::vector<glm::byte> TextureArray::textureData(unsigned int layer) const
{
    std::vector<glm::byte> textureData(mWidth * mHeight * sizeOfGLInternalFormat(mInternalFormat));    
    glGetTextureSubImage(mId, 0, 0, 0, layer, mWidth, mHeight, 1, mFormat, mType, textureData.size(), textureData.data());
    return textureData; 
}


void TextureArray::clear(glm::vec4 color, const std::list<unsigned int>& layers, bool continuous)
{
    if(layers.size())
    {        
        //glBindTexture(mTarget, mId);
        if(continuous)
        {
            switch(mType)                        
            {
                case GL_FLOAT:
                {
                    glClearTexSubImage(mId, 0, 0, 0, layers.front(), mWidth, mHeight, layers.size(), mFormat, mType, glm::value_ptr(color));
                    break;                    
                }
                case GL_UNSIGNED_BYTE:
                {
                    std::array<GLubyte, 4> clearColor = { static_cast<GLubyte>(color.r * 255), static_cast<GLubyte>(color.g * 255), static_cast<GLubyte>(color.b * 255), static_cast<GLubyte>(color.a * 255)};
                    glClearTexSubImage(mId, 0, 0, 0, layers.front(), mWidth, mHeight, layers.size(), mFormat, mType, clearColor.data());
                    break;
                }
                case GL_INT:
                {
                    std::array<GLint, 4> clearColor = { static_cast<GLint>(color.r * 255), static_cast<GLint>(color.g * 255), static_cast<GLint>(color.b * 255), static_cast<GLint>(color.a * 255)};
                    glClearTexSubImage(mId, 0, 0, 0, layers.front(), mWidth, mHeight, layers.size(), mFormat, mType, clearColor.data());
                    break;
                }
                case GL_UNSIGNED_INT:
                {
                    std::array<GLuint, 4> clearColor = { static_cast<GLuint>(color.r * 255), static_cast<GLuint>(color.g * 255), static_cast<GLuint>(color.b * 255), static_cast<GLuint>(color.a * 255)};
                    glClearTexSubImage(mId, 0, 0, 0, layers.front(), mWidth, mHeight, layers.size(), mFormat, mType, clearColor.data());                    
                    break;
                }
            }                     
        }
        else        
        {
            //TODO: no continuous case
        }
        
        glGenerateTextureMipmap(mId);
    }
}

int TextureArray::commitFreeLayer(Texture* texture)
{
    if(mFreeList.size() > 0)
    {
        auto freeLayer = mFreeList.back();
        mTextures[freeLayer] = texture;
        mFreeList.pop_back();
        
        if(mTarget == GL_TEXTURE_2D_ARRAY)
        {
            if(mSparseness)
                glTexturePageCommitmentEXT(mId, 0, 0, 0, freeLayer, mWidth, mHeight, 1, GL_TRUE);
            
            if(texture->data() != nullptr)
                glTextureSubImage3D(mId, 0, 0, 0, freeLayer, mWidth, mHeight, 1, mFormat, mType, texture->data());
            else
                clear({0, 0, 0, 0}, {freeLayer}, true);                        
        }
        else
        {
            if(texture->data() != nullptr)
                glTextureSubImage2D(mId, 0, 0, freeLayer, mWidth, 1, mFormat, mType, texture->data());
            else
                clear({0, 0, 0, 0}, {freeLayer}, true);
        }
        
        texture->setArray(mId, mIndex, freeLayer);
        texture->setTextureArray(this);
        
        return freeLayer;
    }
    else
    {
        LOG("TextureArray::No Free Layer for texture ", texture->name());
        return -1;
    }
}

void TextureArray::freeLayer(unsigned int layer)
{
    if(mTarget == GL_TEXTURE_2D_ARRAY && mSparseness)
        glTexturePageCommitmentEXT(mId, 0, 0, 0, layer, mWidth, mHeight, 1, GL_FALSE);
//     else if (mTarget == GL_TEXTURE_1D_ARRAY)
//         glTexturePageCommitmentEXT(mId, 0, 0, layer, 0, mWidth, mHeight, 1, GL_FALSE);
    
    mTextures[layer] = nullptr;
    
    mFreeList.push_front(layer);
    mFreeList.sort(std::greater<unsigned int>());
}

std::list<unsigned int> TextureArray::commitFreeLayers(unsigned int layerCount)
{
    std::list<unsigned int> committedLayers;
    
    if(mFreeList.size())
    {    
        size_t count = layerCount;
        if(count > mFreeList.size())        
            count = mFreeList.size();

        committedLayers.splice(committedLayers.begin(), mFreeList, std::prev(mFreeList.end(), count), mFreeList.end());
        mFreeList.erase(std::prev(mFreeList.end(), count), mFreeList.end());
            
        if(mTarget == GL_TEXTURE_2D_ARRAY)
            glTexturePageCommitmentEXT(mId, 0, 0, 0, committedLayers.front(), mWidth, mHeight, committedLayers.size(), GL_TRUE);
//         else if (mTarget == GL_TEXTURE_1D_ARRAY)
//             glTexturePageCommitmentEXT(mId, 0, 0, committedLayers.front(), 0, mWidth, mHeight, committedLayers.size(), GL_TRUE);
                        
        clear({0, 0, 0, 0}, committedLayers, true);
    }
    
    return committedLayers;
}

void TextureArray::freeLayers(std::list< unsigned int >& layers)
{
    for(auto layer: layers)
    {
        if(mTarget == GL_TEXTURE_2D_ARRAY)
            glTexturePageCommitmentEXT(mId, 0, 0, 0, layer, mWidth, mHeight, 1, GL_FALSE);
/*        else if (mTarget == GL_TEXTURE_1D_ARRAY)
            glTexturePageCommitmentEXT(mId, 0, 0, layer, 0, mWidth, mHeight, 1, GL_FALSE); */

        mTextures[layer] = nullptr;
    }        
    
    mFreeList.insert(begin(mFreeList), begin(layers), end(layers));
    mFreeList.sort(std::greater<unsigned int>());
}



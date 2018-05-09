#ifndef TEXTUREARRAY_H
#define TEXTUREARRAY_H

#include "texture.h"
#include "glutils.h"

#include <GL/glew.h>
#include <vector>
#include <list>

class TextureArray 
{
public:


    TextureArray(unsigned int index, unsigned int layerCount, std::vector< Texture* > textures, GLenum target, GLint level, GLenum internalFormat, int width, int height, GLenum format, GLenum type, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, GLboolean sparse = GL_TRUE);
    ~TextureArray();
    
    void updateTexture(Texture *texture, unsigned index);
    void updateTextureData(unsigned int index);
    
    GLuint id() const { return mId; }
    GLuint index() const { return mIndex; }
    GLuint textureUnit() const { return mTexUnit; }
    GLint imageUnit() const { return mImageUnit; }
    GLenum target() const { return mTarget; }
    GLint internalFormat() const { return mInternalFormat; }
    GLsizei width() const { return mWidth; }
    GLsizei height() const { return mHeight; }
    glm::ivec2 dimensions() const { return glm::ivec2(mWidth, mHeight); }
    GLenum format() const { return mFormat; }
    GLenum type() const { return mType; }
    GLboolean sparse() const { return mSparseness; }
    SamplerType samplerType() const { return mSamplerType; }
    const std::list<unsigned int>& cleaningList() const { return mCleaningList; }
    const std::vector<Texture *>& textures() const { return mTextures; }
    Texture* texture(std::string texture) const;
    std::vector<glm::byte> textureData(unsigned int layer) const;

    bool isEmpty() const { return mFreeList.size() == mLayerCount ? true : false; }
    bool isFull() const { return mFreeList.empty(); }
    
    void clear(glm::vec4 color, const std::list<unsigned int>& layers = {}, bool continuous = false);

    int commitFreeLayer(Texture* texture);
    void freeLayer(unsigned int layer);
    
    //TODO: Update these functions to follow the template of the single version
    std::list< unsigned int > commitFreeLayers(unsigned int layerCount);
    void freeLayers(std::list< unsigned int >& layers);
    
    void setSamplerType(SamplerType type){ mSamplerType = type; }
    void bindToTextureUnit(GLuint unit) { mTexUnit = unit; }
    void setImageUnit(GLint unit) { mImageUnit = unit; }
    
private:
    unsigned int mIndex;
    unsigned int mTexUnit;
    int mImageUnit = -1;
    unsigned int mLayerCount;
    std::vector<Texture *> mTextures;
    std::list<unsigned int> mFreeList;
    std::list<unsigned int> mCleaningList;
    
    GLuint mId;    
    GLenum mTarget;
    GLenum mMinFilter;
    GLenum mMagFilter;
    
    GLfloat mAFLevel;
 
    GLint mLevel;
    GLint mInternalFormat;
    GLsizei mWidth;
    GLsizei mHeight;
    GLenum mFormat;
    GLenum mType;
    GLboolean mSparseness;
    SamplerType mSamplerType;
};

#endif // TEXTUREARRAY_H

#include "textureview.h"

TextureView::TextureView(std::string name, Texture* origTexture, GLenum target, GLenum internalFormat, GLuint minLevel, GLuint numLevels, GLuint minLayer, GLuint numLayers)
:
    mName(name),
    mTarget(target),
    mOriginalTexture(origTexture),
    mInternalFormat(internalFormat),
    mMinLevel(minLevel),
    mNumLevels(numLevels),
    mMinLayer(minLayer),
    mNumLayers(numLayers)
{
    glGenTextures(1, &mId);
    
    if(glIsTexture(origTexture->textureArrayId()))
        glTextureView(mId, mTarget, origTexture->textureArrayId(), mInternalFormat, mMinLevel, mNumLevels, mMinLayer, mNumLayers);
    else if(origTexture->generated())
        glTextureView(mId, mTarget, origTexture->id(), mInternalFormat, mMinLevel, mNumLevels, 0, 1);
}


TextureView::~TextureView()
{

}

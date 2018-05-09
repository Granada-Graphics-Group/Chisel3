#ifndef TEXTUREVIEW_H
#define TEXTUREVIEW_H

#include "texture.h"

class TextureView
{
public:
    TextureView(std::string name, Texture* origTexture, GLenum target, GLenum internalFormat, GLuint minLevel, GLuint numLevels, GLuint minLayer, GLuint numLayers);

    ~TextureView();

    std::string name() const { return mName; }
    GLuint id() const { return mId; }
    GLenum target() const { return mTarget; }
    GLint internalFormat() const { return mInternalFormat; }
    Texture* originalTexture() const { return mOriginalTexture; }
private:
    std::string mName;

    GLuint mId;
    GLenum mTarget;
    GLenum mInternalFormat;
    GLuint mMinLevel;
    GLuint mNumLevels;
    GLuint mMinLayer;
    GLuint mNumLayers;
    
    Texture* mOriginalTexture;
};

#endif // TEXTUREVIEW_H

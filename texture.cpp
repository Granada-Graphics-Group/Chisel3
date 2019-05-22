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

#include "texture.h"
#include "texturearray.h"
#include "algorithm"

Texture::~Texture()
{
    if(mGenerated && glIsTexture(mId))
       glDeleteTextures(1, &mId);
}
//TODO Manejar mejor la actualización de parametros. Decidir aquellos por defecto en el constructor
Texture::Texture(std::string name, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const std::vector<glm::byte>& data, GLenum magFilter, GLenum minFilter, GLenum anitropyLevel, bool generate)
:
    mName(name),
    mId(-1),
    mTarget(target),
    mLevel(0),
    mInternalFormat(internalFormat),
    mWidth(width),
    mHeight(height),
    mFormat(format),
    mType(type),
    mData(data),
    mMinFilter(minFilter),
    mMagFilter(magFilter),
    mAFLevel(anitropyLevel),
    mGenerated(generate)
{   
    if(generate)
    {
        glGenTextures(1, &mId);

        glBindTexture(mTarget, mId);

        glTexParameterf(mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        float color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        glTexParameterfv(mTarget, GL_TEXTURE_BORDER_COLOR, color);
    
        glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, mMagFilter);
        glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, mMinFilter);
    
        if(mAFLevel > 0)
            glTexParameteri(mTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, mAFLevel);    
    
        //glTexImage2D( mTarget, 0, internalFormat, width, height, 0, format, type, data);
        
        GLint levels = static_cast<GLint>(log2(std::max(mWidth, mHeight)) + 1); 
        glTexStorage2D(mTarget, levels, internalFormat, width, height);
        
        if (mData.size() > 0)
            glTexSubImage2D(mTarget, 0, 0, 0, mWidth, mHeight, mFormat, mType, mData.data());
    
        glGenerateMipmap( mTarget );                
    }
}

std::vector<glm::byte> Texture::textureData() const
{
    return mArray->textureData(mArrayIndices.y);
}


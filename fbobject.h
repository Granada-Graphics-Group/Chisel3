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

#ifndef FBOBJECT_H
#define FBOBJECT_H

#include <GL/glew.h>
#include <vector>
#include <array>

#include "texture.h"

class FBObject
{
public:
    FBObject(GLuint defaultID) : mId(defaultID) {};
    FBObject(GLenum target, std::vector<Texture* > color) : FBObject(target, color, nullptr){};
    FBObject(GLenum target, std::vector<Texture* > color, Texture* depth) : FBObject(target, color, depth, nullptr){};
    FBObject(GLenum target, std::vector<Texture* > color, Texture* depth, Texture* stencil) : FBObject(target, color, depth, stencil, nullptr){};
    FBObject(GLenum target, std::vector<Texture* > color, Texture* depth, Texture* stencil, Texture* depthstencil);
    ~FBObject();

    GLuint id() const { return mId;}
    GLenum target() const { return mTarget; }
    GLsizei width() const { return mDimensions[0]; }
    GLsizei height() const { return mDimensions[1]; }
    
    std::vector<GLenum> drawBuffers() const { return mDrawBuffers; }
    const std::vector<Texture *> colorAttachments() const { return mColorAttachments; }    
    Texture* colorAttachment(int index) const;
    unsigned int colorAttachmentCount() const { return static_cast<unsigned int>(mColorAttachments.size()); }
    Texture* depthAttachment() const { return mDepthAttachment; }
    Texture* stencilAttachment() const { return mStencilAttachment; }
    Texture* depthStencilAttachment() const { return mDepthAttachment; }
    
    void bindFBO() { glBindFramebuffer(mTarget, mId); }
    void unbindFBO() { glBindFramebuffer(mTarget, 0); }
    bool checkCompleteness();
    
    void setId(GLuint id){ mId = id; }
    void setTarget(GLenum target){ mTarget = target; }
    void setDrawBuffers(std::vector<GLenum> drawBuffers);
    void setcolorAttachment(Texture* color, int index);
    void setDepthAttachment(Texture* depth);
    void setStencilAttachment(Texture* stencil);
    void setDepthStencilAttachment(Texture* depthStencil);
    
private:
    void attachTexture(GLenum attachment, Texture* texture);
    void detachTexture(GLenum attachment);
    void updateDimensions(GLsizei width, GLsizei height);
    
    static GLint maxColorAttachs;
    
    GLuint mId = 0;
    GLenum mTarget = GL_FRAMEBUFFER;
    GLenum mStatus = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;

    std::array<GLsizei, 2> mDimensions = {{0, 0}};//VS15 {{0, 0}};

    std::vector<GLenum> mDrawBuffers;    
    std::vector<Texture* > mColorAttachments;
    Texture* mDepthAttachment = nullptr;
    Texture* mStencilAttachment = nullptr;
    Texture* mDepthStencilAttachment = nullptr;
};

#endif // FBOBJECT_H

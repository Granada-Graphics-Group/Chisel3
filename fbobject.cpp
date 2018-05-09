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

#include "fbobject.h"
#include "logger.hpp"

#include <iostream>
#include <algorithm>

GLint FBObject::maxColorAttachs = -1;

FBObject::FBObject(GLenum target, std::vector< Texture* > color, Texture* depth, Texture* stencil, Texture* depthstencil)
:
    mTarget(target),
    mColorAttachments(color),
    mDepthAttachment(depth),
    mStencilAttachment(stencil),
    mDepthStencilAttachment(depthstencil)
{    
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &FBObject::maxColorAttachs);
    
    glCreateFramebuffers(1, &mId);
    
    size_t colorAttachments = 0;
    
    if(!mColorAttachments.size())
        mDrawBuffers.push_back(GL_NONE);
    else if(mColorAttachments.size() <= FBObject::maxColorAttachs)
        colorAttachments = mColorAttachments.size();
    else
    {
        colorAttachments = FBObject::maxColorAttachs;
        LOG_ERR("FBO ", mId, " Maximum number of color attachments exceeded");
    }
        
    for(int i = 0; i < colorAttachments; ++i)
    {
        attachTexture(GL_COLOR_ATTACHMENT0 + i, mColorAttachments[i]);
        updateDimensions(mColorAttachments[i]->width(), mColorAttachments[i]->height());
        mDrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    
    if(mDepthAttachment != nullptr)
    {
        attachTexture(GL_DEPTH_ATTACHMENT, mDepthAttachment);
        updateDimensions(mDepthAttachment->width(), mDepthAttachment->height());
    }
    
    if(mStencilAttachment != nullptr)
    {
        attachTexture(GL_STENCIL_ATTACHMENT, mStencilAttachment);
        updateDimensions(mStencilAttachment->width(), mStencilAttachment->height());
    }
    
    if(mDepthStencilAttachment != nullptr)
    {
        attachTexture(GL_DEPTH_STENCIL_ATTACHMENT, mDepthStencilAttachment);
        updateDimensions(mDepthStencilAttachment->width(), mDepthStencilAttachment->height());
    }
    
    checkCompleteness();
    
    glNamedFramebufferDrawBuffers(mId, mDrawBuffers.size(), &mDrawBuffers[0]);      
}


FBObject::~FBObject()
{
    for(int i = 0; i < mColorAttachments.size(); ++i)
        detachTexture(GL_COLOR_ATTACHMENT0 + i);
    
    if(mDepthAttachment) detachTexture(GL_DEPTH_ATTACHMENT);
    
    if(mStencilAttachment) detachTexture(GL_STENCIL_ATTACHMENT);
    
    if(mDepthStencilAttachment) detachTexture(GL_DEPTH_STENCIL_ATTACHMENT);
    
    glDeleteFramebuffers(1, &mId);
}

// *** Public Methods *** //

Texture* FBObject::colorAttachment(int index) const
{
    if(index >=0 && index < mColorAttachments.size())
        return mColorAttachments[index];
    else
        return nullptr;
}


bool FBObject::checkCompleteness()
{
    bool valid = false;

    mStatus = glCheckNamedFramebufferStatus(mId, mTarget);

    switch(mStatus)
    {
    case GL_FRAMEBUFFER_COMPLETE:
        valid = true;
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        LOG_ERR("FBO ", mId, ": Incomplete Attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        LOG_ERR("FBO ", mId, ": Incomplete Missing Attachment");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        LOG_ERR("FBO ", mId, ": Incomplete draw buffer");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        LOG_ERR("FBO ", mId, ": Incomplete read buffer");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        LOG_ERR("FBO ", mId, ": Frambuffer unsupported");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        LOG_ERR("FBO ", mId, ": Incomplete multisample");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        LOG_ERR("FBO ", mId, ": Incomplete layer targets");
        break;        
    default:
        LOG_ERR("FBO ", mId, ": Unknown error");
    }
    
    return valid;
}

void FBObject::setDrawBuffers(std::vector< GLenum > drawBuffers)
{
    mDrawBuffers.clear();
    
    for(auto bufferName : drawBuffers)
    {
        int index = bufferName - GL_COLOR_ATTACHMENT0;
        
        if(mColorAttachments.at(index))
            mDrawBuffers.push_back(bufferName);
        else
            LOG_ERR("FBO ", mId, ": Setting draw buffers. No color attachment at index ", index);
    }
}


void FBObject::setcolorAttachment(Texture* color, int index)
{
    if (index >= 0 && index < FBObject::maxColorAttachs)
    {
        if (mColorAttachments.size() == 0)
            mDrawBuffers.pop_back();

        if(mColorAttachments.size() <= index)
            mColorAttachments.resize(index + 1);
                       
        mColorAttachments[index] = color;
        attachTexture(GL_COLOR_ATTACHMENT0 + index, mColorAttachments[index]);
        updateDimensions(mColorAttachments[index]->width(), mColorAttachments[index]->height());
        
        if(std::find(begin(mDrawBuffers), end(mDrawBuffers), GL_COLOR_ATTACHMENT0 + index) == end(mDrawBuffers))
            mDrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + index);        
    }
    
    checkCompleteness();
    
    glNamedFramebufferDrawBuffers(mId, mDrawBuffers.size(), &mDrawBuffers[0]);
}

void FBObject::setDepthAttachment(Texture* depth)
{   
    mDepthAttachment = depth;
    attachTexture(GL_DEPTH_ATTACHMENT, mDepthAttachment);
    updateDimensions(mDepthAttachment->width(), mDepthAttachment->height());
    
    checkCompleteness();
}

void FBObject::setStencilAttachment(Texture* stencil)
{    
    mStencilAttachment = stencil;
    attachTexture(GL_STENCIL_ATTACHMENT, mStencilAttachment);
    updateDimensions(mStencilAttachment->width(), mStencilAttachment->height());
    
    checkCompleteness();
}

void FBObject::setDepthStencilAttachment(Texture* depthStencil)
{
    mDepthStencilAttachment = depthStencil;    
    attachTexture(GL_DEPTH_STENCIL_ATTACHMENT, mDepthStencilAttachment);
    updateDimensions(mDepthStencilAttachment->width(), mDepthStencilAttachment->height());
    
    checkCompleteness();
}

// *** Private Methods *** //

void FBObject::attachTexture(GLenum attachment, Texture* texture)
{
    if(!glIsTexture(texture->textureArrayId()))        
        glNamedFramebufferTexture(mId, attachment, texture->id(), texture->level());
    else
        glNamedFramebufferTextureLayer(mId, attachment, texture->textureArrayId(), texture->level(), texture->textureArrayLayerIndex());    
}

void FBObject::detachTexture(GLenum attachment)
{
    glNamedFramebufferTexture(mId, attachment, 0, 0);
}

void FBObject::updateDimensions(GLsizei width, GLsizei height)
{
//     if(width < mDimensions.front() || mDimensions[0] == 0)
//         mDimensions[0] = width;
//     if(height < mDimensions.back() || mDimensions[1] == 0)
//         mDimensions[1] = height;
    if(width > 0 && height > 0)
        mDimensions = {{width, height}};
    
}


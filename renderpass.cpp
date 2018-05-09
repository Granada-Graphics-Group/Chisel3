#include "renderpass.h"

RenderPass::RenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates, bool depthTest, GLenum depthFunc)
:
    mName(name),
    mScene(scene),
    mSceneMaterial(sceneMaterial),
    mBlendingStates(blendingStates),
    mDepthTest(depthTest),
    mDepthFunc(depthFunc)
{
    if(mSceneMaterial != nullptr)
    {
        mUsingSceneMaterial = true;
        mBlendingStates.resize(mSceneMaterial->shader()->outputs().size());
    }
}

RenderPass::~RenderPass()
{

}


// *** Public Methods *** //

bool RenderPass::isBlendingEnabled(GLint indexBuffer)
{
    if(indexBuffer < mBlendingStates.size())
        return mBlendingStates[indexBuffer].mEnabled;
    else
        return false;
}

const BlendingState& RenderPass::blendingState(GLint indexBuffer)
{
    if(indexBuffer < mBlendingStates.size())
        return mBlendingStates[indexBuffer];
}

bool RenderPass::enableBlending(GLint indexBuffer, const BlendingState& state)
{
    if(indexBuffer < static_cast<GLint>(mBlendingStates.size()))
    {
        mBlendingStates[indexBuffer] = state;

        if(mBlendingStates[indexBuffer].mEnabled != true) 
            mBlendingStates[indexBuffer].mEnabled = true;
        
        return true;
    }
    
    return false;
}

bool RenderPass::enableBlending(GLint indexBuffer)
{
    if(indexBuffer < static_cast<GLint>(mBlendingStates.size()))
    {
        mBlendingStates[indexBuffer].mEnabled = true;
        return true;
    }
    
    return false;
}

bool RenderPass::disableBlending(GLint indexBuffer)
{
    if(indexBuffer < static_cast<GLint>(mBlendingStates.size()))
    {
        mBlendingStates[indexBuffer].mEnabled = false;
        return true;
    }
    
    return false;
}


void RenderPass::setSceneMaterial(Material* sceneMaterial)
{
    if(sceneMaterial != nullptr)
    {
        mUsingSceneMaterial = true;
        mSceneMaterial = sceneMaterial;
    }
}

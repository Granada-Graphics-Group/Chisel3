#include "rendertarget.h"


// RenderTarget::RenderTarget(std::string name, FBObject* target, std::array<GLint, 4> viewport)
// :
//     mName(name),
//     mFBO(target),
//     mViewport(viewport)
// {
//     
// }

RenderTarget::RenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, bool defaultTarget, GLuint defaultTargetID)
:
    mName(name),
    mResourceManager(manager),
    mViewport(viewport),
    mPasses(passes),
    mUseDefaultTarget(defaultTarget),
    mDefaultTargetID(defaultTargetID)
{
    if(!mUseDefaultTarget)
        createTextureTargets(mPasses.back());
    else
        mFBO = std::make_unique<FBObject>(defaultTargetID);
}

RenderTarget::RenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, const std::vector< Texture* >& colorTargets, Texture* depthTarget, bool defaultTarget, GLuint defaultTargetID)
:
    mName(name),
    mResourceManager(manager),
    mViewport(viewport),
    mPasses(passes),
    mColorTextures(colorTargets),
    mDepthTexture(depthTarget),
    mUseDefaultTarget(defaultTarget),
    mDefaultTargetID(defaultTargetID)
{
    if(!mUseDefaultTarget)
    {
        attachColorTexturesToFBO();
        
        auto enableDepth = false;
        for(const auto& pass : passes)
            if(pass->isDepthTestEnabled())
                enableDepth = true;
        
        if(enableDepth && depthTarget == nullptr)
        {
            std::string textureName = mName + "Depth";
            mDepthTexture = mResourceManager->createTexture(textureName, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mViewport[2], mViewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);                
        }
    }
    else
        mFBO = std::make_unique<FBObject>(defaultTargetID);
}



RenderTarget::~RenderTarget()
{

}


//*** Public Methods *** //
void RenderTarget::addPass(RenderPass* pass, bool defaultTarget)
{
    if(pass != nullptr)
    {
        mPasses.push_back(pass);

        if(!defaultTarget)
            createTextureTargets(pass);
    }
}

void RenderTarget::setFBO(std::unique_ptr<FBObject> fbo)
{
    mFBO = std::move(fbo);
    mViewport = { { 0, 0, fbo->width(), fbo->height() } }; //VS15
}

void RenderTarget::setColorTextures(const std::vector< Texture* >& colorTextures, bool updateDepthTexture)
{
    mColorTextures = colorTextures;
    
    if(!mUseDefaultTarget)
    {
        mViewport = { { 0, 0, mColorTextures.back()->width(), mColorTextures.back()->height() } };
        attachColorTexturesToFBO(updateDepthTexture);        
    }
}

void RenderTarget::setDepthTexture(Texture* depthTexture)
{
    mDepthTexture = depthTexture;
    
    if(!mUseDefaultTarget && depthTexture != nullptr)
    {
        mViewport = { { 0, 0, mDepthTexture->width(), mDepthTexture->height() } };
        mFBO->setDepthAttachment(mDepthTexture);
        mFBO->checkCompleteness();
    }    
}


void RenderTarget::attachColorTexturesToFBO(bool updateDepthTexture)
{
    if(mColorTextures.size())
    {
        if(mFBO == nullptr)
            mFBO = std::make_unique<FBObject>(GL_FRAMEBUFFER, mColorTextures, mDepthTexture);
        else
        {
            for(int i = 0; i < mColorTextures.size(); ++i)              
                mFBO->setcolorAttachment(mColorTextures[i], i);
            
            if(updateDepthTexture)
            {
                mResourceManager->deleteTexture(mDepthTexture);
                mDepthTexture = mResourceManager->createTexture(mName + "Depth", GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mViewport[2], mViewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);
            }
            
            mFBO->setDepthAttachment(mDepthTexture);

            mFBO->checkCompleteness();
        }
        
//         mColorViews.clear();
//         mDepthView.reset();
        
//         for(auto& color: mColorTextures)            
//             mColorViews.push_back(std::make_unique<TextureView>(color->name() + "View", color, GL_TEXTURE_2D, color->internalFormat(), 0, 1, color->textureArrayLayerIndex(), 1));
//         
//         if(mDepthView == nullptr)
//             mDepthView = std::make_unique<TextureView>(mDepthTexture->name() + "View", mDepthTexture, GL_TEXTURE_2D, mDepthTexture->internalFormat(), 0, 1, mDepthTexture->textureArrayLayerIndex(), 1);
    }
    else
    {
        if (mFBO == nullptr)
            mFBO = std::make_unique<FBObject>(GL_FRAMEBUFFER, mColorTextures, mDepthTexture);
    }
}


void RenderTarget::updateUniformData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
    
    if((size + offset) > mUniformData.size())
        mUniformData.resize(size + offset);
    
    std::copy(byteData, byteData + size, begin(mUniformData) + offset);    
}

//*** Private Methods ***//
void RenderTarget::createTextureTargets()
{
    int currentMaxIndex = 0;
    std::vector<int> currentTypeTargets;
    
    for (const auto& pass: mPasses)
    {
        auto blendingStates = pass->blendingStates();
                
        for(const auto& blendingState: pass->blendingStates())
            if(currentMaxIndex < blendingState.mIndexBuffer)
                currentMaxIndex = blendingState.mIndexBuffer;
        
        if(currentTypeTargets.size() < currentMaxIndex)
            currentTypeTargets.resize(currentMaxIndex, -1);

        for(const auto& blendingState: blendingStates)
            currentTypeTargets[blendingState.mIndexBuffer] = blendingState.mEnabled ? 1 : 0;
    }

    mColorTextures.resize(currentTypeTargets.size());
    
    for(auto index = 0; index < currentTypeTargets.size(); ++index)
        if(currentTypeTargets[index] != -1)
        {
            std::string textureName = mName + "Color" + std::to_string(index);

            if(currentTypeTargets[index] == 1)
                mColorTextures[index] = mResourceManager->createTexture(textureName, GL_TEXTURE_2D, GL_RGBA8, mViewport[2], mViewport[3], GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false); //std::make_unique<Texture>(textureName, GL_TEXTURE_2D, GL_RGBA8, mViewport[2], mViewport[3], GL_RGBA, GL_UNSIGNED_BYTE, nullptr, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false); 
            else
                mColorTextures[index] = mResourceManager->createTexture(textureName, GL_TEXTURE_2D, GL_RGB8, mViewport[2], mViewport[3], GL_RGB, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false);  //std::make_unique<Texture>(textureName, GL_TEXTURE_2D, GL_RGB8, mViewport[2], mViewport[3], GL_RGB, GL_UNSIGNED_BYTE, nullptr, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false); 
            
            //mResourceManager->insertTexture(mColorTextures[index].get());
        }
}


void RenderTarget::createTextureTargets(RenderPass* pass)
{
    size_t colorOutputs = 0;
    bool depth = true;    
            
    if(pass->usingSceneMaterial())
        colorOutputs = pass->sceneMaterial()->shader()->outputs().size();            
    else
    {
        for( auto& material: pass->scene()->materials())
        {
            auto tempOutputs = material->shader()->outputs().size();
                    
            if(tempOutputs > colorOutputs)
                colorOutputs = tempOutputs;
        }
    }
            
    for(int i = 0; i < colorOutputs; ++i)
    {
        std::string textureName = mName + "Color" + std::to_string(i);
        auto newColorTexture = mResourceManager->createTexture(textureName, GL_TEXTURE_2D, GL_RGBA8, mViewport[2], mViewport[3], GL_RGBA, GL_UNSIGNED_BYTE, {}, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false);
        mColorTextures.push_back(newColorTexture);
//         mColorTextures.push_back(std::make_unique<Texture>(textureName, GL_TEXTURE_2D, GL_RGBA8, mViewport[2], mViewport[3], GL_RGBA, GL_UNSIGNED_BYTE, nullptr, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, 8, false)); 
//         mResourceManager->insertTexture(mColorTextures.back().get());
    }
            
    if(depth)
    {
        std::string textureName = mName + "Depth";
        mDepthTexture = mResourceManager->createTexture(textureName, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, mViewport[2], mViewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, {}, GL_NEAREST, GL_NEAREST, 8, false);
        //mResourceManager->insertTexture(mDepthTexture);                
    }
    
    if(mFBO == nullptr)
        mFBO = std::make_unique<FBObject>(GL_FRAMEBUFFER, mColorTextures, mDepthTexture);    
}


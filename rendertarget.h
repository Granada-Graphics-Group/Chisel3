#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "renderpass.h"
#include "fbobject.h"
#include "resourcemanager.h"
#include "textureview.h"

#include <vector>

class RenderTarget
{
public:
// 	RenderTarget(std::string name, FBObject* target) : RenderTarget(name, target, { { 0, 0, target->width(), target->height() } }) {}; //VS15
//     RenderTarget(std::string name, FBObject* target, std::array<GLint, 4> viewport);
    RenderTarget(std::string name, ResourceManager* manager, std::array<GLint, 4> viewport): RenderTarget(name, manager, viewport, {}, false){};
    RenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, bool defaultTarget, GLuint defaultTargetID = 0);
    RenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, const std::vector< Texture* >& colorTargets, bool defaultTarget, GLuint defaultTargetID = 0): RenderTarget(name, manager,viewport, passes, colorTargets, nullptr, defaultTarget, defaultTargetID){};
    RenderTarget(std::string name, ResourceManager* manager, std::array< GLint, int(4) > viewport, const std::vector< RenderPass* >& passes, const std::vector< Texture* >& colorTargets, Texture* depthTarget, bool defaultTarget, GLuint defaultTargetID = 0);
    ~RenderTarget();

    const std::string& name() const { return mName; }
    const std::vector<RenderPass* >& passes() const { return mPasses; }
    FBObject* fbo() const { return mFBO.get(); }
    const std::array<GLint, 4>& viewport() const { return mViewport; }
    const std::vector<std::unique_ptr<TextureView>>& colorOutputs() const { return mColorViews; }
    const std::vector<Texture*>& colorTexOutputs() const { return mColorTextures; }
    const std::unique_ptr<TextureView>& depthOutput() const { return mDepthView; }
    const Texture* depthTexOutput() const { return mDepthTexture; }
    const std::vector<GLint>& colorInputs() const { return mColorInputs; }
    GLint depthInput() const { return mDepthInput; }
    const std::vector<GLuint>& colorInputIndices() const { return mColorInputIndices; }
    const std::vector<GLuint>& depthInputIndices() const { return mDepthInputIndices; }
    bool isDefaultTargetUsed() const { return mUseDefaultTarget; }
    
    void setFBO(std::unique_ptr<FBObject> fbo);
    void setColorTextures(const std::vector< Texture* >& colorTextures, bool updateDepthTexture = true);
    void setDepthTexture(Texture* depthTexture);
    void setViewport(GLint x, GLint y, GLsizei width, GLsizei height) { mViewport = { { x, y, width, height } }; } //VS15
    void setResourceManager(ResourceManager* manager) { mResourceManager = manager; }
    void setColorInputs(std::vector<GLint> colorInputs) { mColorInputs = colorInputs; }
    void setDepthInput(GLint depthInput){ mDepthInput = depthInput; }
    
    void setColorInputIndices(const std::vector<GLuint>& indices) { mColorInputIndices = indices; }
    void setDepthInputIndices(const std::vector<GLuint>& indices) { mDepthInputIndices = indices; }

    
    void addPass(RenderPass* pass, bool defaultTarget);
    void removePass(RenderPass* pass);
    
    void attachColorTexturesToFBO(bool updateDepthTexture = false);
private:
    void createTextureTargets();
    void createTextureTargets(RenderPass* pass);
    
    std::string mName;
    bool mUseDefaultTarget = false;
    GLuint mDefaultTargetID = 0;
    std::array<GLint, 4> mViewport;
    
    std::vector<RenderPass* > mPasses;
    ResourceManager* mResourceManager = nullptr;
    std::unique_ptr<FBObject> mFBO = nullptr;
    
    std::vector< Texture* > mColorTextures;
    Texture* mDepthTexture = nullptr;

    std::vector<std::unique_ptr<TextureView>> mColorViews;
    std::unique_ptr<TextureView> mDepthView = nullptr;
    
    std::vector<GLint> mColorInputs; // deprecated
    GLint mDepthInput = -1;          // deprecated
    
    std::vector<GLuint> mColorInputIndices;
    std::vector<GLuint> mDepthInputIndices;
};

#endif // RENDERTARGET_H

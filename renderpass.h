#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <string>
#include <array>

#include "material.h"
#include "scene3d.h"

struct BlendingState
{
    BlendingState(){ mBlendEquation = { { GL_FUNC_ADD, GL_FUNC_ADD } }; mBlendFunc = { { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO } }; }

    bool mEnabled = false;
    GLint mIndexBuffer = 0;
    
    std::array<GLenum, 2> mBlendEquation;
    std::array<GLenum, 4> mBlendFunc;
};

class RenderPass
{
public:
    RenderPass(std::string name) : RenderPass(name, nullptr) {};
    RenderPass(std::string name, Scene3D* scene) :  RenderPass(name, scene, nullptr) {};
    RenderPass(std::string name, Scene3D* scene, Material* sceneMaterial): RenderPass(name, scene, sceneMaterial, {BlendingState()}) {};
    RenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates): RenderPass(name, scene, sceneMaterial, blendingStates, true) {};
    RenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates, bool depthTest): RenderPass(name, scene, sceneMaterial, blendingStates, depthTest, GL_LESS) {};
    RenderPass(std::string name, Scene3D* scene, Material* sceneMaterial, std::vector<BlendingState> blendingStates, bool depthTest, GLenum depthFunc);
    ~RenderPass();
    
    std::string name() { return mName; }
    Scene3D* scene() { return mScene; }
    Material* sceneMaterial() { return mSceneMaterial; }
    bool isDepthTestEnabled() { return mDepthTest; }
    bool isBlendingEnabled(GLint indexBuffer);
    GLenum depthFunc() { return mDepthFunc; }
    const std::vector<BlendingState>& blendingStates() { return mBlendingStates; }
    const BlendingState& blendingState(GLint indexBuffer);
    
    bool usingSceneMaterial() { return mUsingSceneMaterial; }    
    void setSceneMaterial(Material* sceneMaterial);
        
    bool enableBlending(GLint indexBuffer, const BlendingState& state);
    bool enableBlending(GLint indexBuffer);
    bool disableBlending(GLint indexBuffer);
    
    void disableDepthTest(){ mDepthTest = false; }
    void enableDepthTest(GLenum depthFunc = GL_LESS){ mDepthTest = true; mDepthFunc = depthFunc; }
    
    void setScene(Scene3D* scene){ mScene = scene; }
    
    void setAutoClear(bool clear){ mAutoclear = clear; }
    void setAutoClearColor(bool clear){ mAutoClearColor = clear; }
    void setAutoClearDepth(bool clear){ mAutoClearDepth = clear; }
    void setAutoClearStencil(bool clear){ mAutoClearStencil = clear; }
    
    bool autoClear(){ return mAutoclear; }
    bool autoClearColor(){ return mAutoClearColor; }
    bool autoClearDepth(){ return mAutoClearDepth; }
    bool autoClearStencil(){ return mAutoClearStencil; }
    
private:
    std::string mName;
    
    bool mUsingSceneMaterial = false;
    Material* mSceneMaterial = nullptr;

    Scene3D* mScene = nullptr;
    std::vector<Model3D* > mModels;
    
    std::vector<BlendingState> mBlendingStates;
    bool mDepthTest = true;
    GLenum mDepthFunc = GL_LESS;
    
    bool mAutoclear = true;
    bool mAutoClearColor = true;
    bool mAutoClearDepth = true;
    bool mAutoClearStencil = true;
};

#endif // RENDERPASS_H

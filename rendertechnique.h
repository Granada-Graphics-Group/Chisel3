#ifndef RENDERTECHNIQUE_H
#define RENDERTECHNIQUE_H

#include "rendertarget.h"

#include <vector>
#include <string>
#include <set>

class GLRenderer;

class RenderTechnique
{
public:
    RenderTechnique(std::string name): RenderTechnique(name, {}) {};
    RenderTechnique(std::string name, std::vector<RenderTarget *> targets): RenderTechnique(name, targets, -1) {};
    RenderTechnique(std::string name, std::vector<RenderTarget *> targets, int life);
    ~RenderTechnique();
    
    std::string name() const { return mName; }
    GLRenderer* renderer() const { return mRenderer; }
    std::vector<RenderTarget* > targets() const { return mTargets; }
    const std::set<Texture*> disposableTextures() const { return mDisposableTextures; }
    int life() const { return mLife; }
        
    void addTarget(RenderTarget* target);
    void setTarget(RenderTarget* target, int index);
    void removeTarget(RenderTarget* target);
    
    void setRenderer(GLRenderer* renderer) { mRenderer = renderer; }
    void settingUP();
    
    void addDisposableTexture(Texture* texture);
    void clearDisposableTextures();

    void setLife(int life){ mLife = life; }
    void decreaseLife(){ if(mLife > -1) mLife--; }
    
private:
    std::string mName;
    GLRenderer* mRenderer = nullptr;
    std::vector<RenderTarget*> mTargets;
    std::set<Texture*> mDisposableTextures;
    
    int mLife = -1;
};

#endif // RENDERTECHNIQUE_H


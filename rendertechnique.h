#ifndef RENDERTECHNIQUE_H
#define RENDERTECHNIQUE_H

#include "rendertarget.h"

#include <vector>
#include <string>

class GLRenderer;

class RenderTechnique
{
public:
    RenderTechnique(std::string name): RenderTechnique(name, {}) {};
    RenderTechnique(std::string name, std::vector<RenderTarget *> targets): RenderTechnique(name, targets, -1) {};
    RenderTechnique(std::string name, std::vector<RenderTarget *> targets, int life);
    ~RenderTechnique();
    
    std::string name(){ return mName; }
    GLRenderer* renderer(){ return mRenderer; }
    std::vector<RenderTarget* > targets() { return mTargets; }
    int life(){ return mLife; }
    
    void setLife(int life){ mLife = life; }
    void decreaseLife(){ if(mLife > -1) mLife--; }
    
    void addTarget(RenderTarget* target);
    void setTarget(RenderTarget* target, int index);
    void removeTarget(RenderTarget* target);
    
    void setRenderer(GLRenderer* renderer) { mRenderer = renderer; }
    void settingUP();
    
private:
    std::string mName;
    GLRenderer* mRenderer = nullptr;
    std::vector<RenderTarget* > mTargets;    
    int mLife = -1;
};

#endif // RENDERTECHNIQUE_H


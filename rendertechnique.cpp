#include "rendertechnique.h"
#include "glrenderer.h"

#include <algorithm>

/*RenderTechnique::RenderTechnique(std::string name, std::vector< RenderTarget* > targets)
:
    mName(name),
    mTargets(targets)
{
    if(targets.size())
        settingUP();
}*/


RenderTechnique::RenderTechnique(std::string name, std::vector< RenderTarget* > targets, int life)
:
    mName(name),
    mTargets(targets),
    mLife(life)
{
    if(targets.size())
        settingUP();
}

RenderTechnique::~RenderTechnique()
{

}


// *** Public Methods *** //

void RenderTechnique::addTarget(RenderTarget* target)
{
    auto found = std::find(begin(mTargets), end(mTargets), target);
    
    if(found == end(mTargets))
    {
        mTargets.push_back(target);
        
        for(const auto pass: target->passes())
            mRenderer->insertScene(pass->scene());
    }
}

void RenderTechnique::setTarget(RenderTarget* target, int index)
{
    mTargets[index] = target;
    settingUP();
}


void RenderTechnique::removeTarget(RenderTarget* target)
{
    auto found = std::find(begin(mTargets), end(mTargets), target);
    
    if(found != end(mTargets))
        mTargets.erase(found);
}

void RenderTechnique::settingUP()
{
    for(int i = 1; i < mTargets.size(); ++i)        
    {
        auto previuos = mTargets[i - 1];
        auto current = mTargets[i];
        
        if(!previuos->isDefaultTargetUsed())
        {
            std::vector<GLint> colorOutputs;
            std::vector<GLuint> colorOutputIndices;
        
//             for(auto &colorOutput : previuos->colorOutputs())
//                 colorOutputs.push_back(colorOutput->id());
            
            for(auto &colorOutput : previuos->colorTexOutputs())
            {
                colorOutputIndices.push_back(colorOutput->textureArrayIndex());
                colorOutputIndices.push_back(colorOutput->textureArrayLayerIndex());                
            }
                
//             current->setColorInputs(colorOutputs);
            current->setColorInputIndices(colorOutputIndices);
//             current->setDepthInput(previuos->depthOutput()->id());
            current->setDepthInputIndices({previuos->depthTexOutput()->textureArrayIndex(), previuos->depthTexOutput()->textureArrayLayerIndex()});
        }
        
// //         auto name = previuos->depthOutput()->name();
// //         auto id = previuos->depthOutput()->id();
// //         auto depthInputId = current->depthInput();
// //         
// //         TextureView *depthTex = previuos->depthOutput().get();
    }
}

void RenderTechnique::addDisposableTexture(Texture* texture)
{
    if(texture != nullptr)
        mDisposableTextures.insert(texture);
}

void RenderTechnique::clearDisposableTextures()
{
    mDisposableTextures.clear();
}

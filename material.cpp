#include "material.h"
#include "scene3d.h"

// #define GLM_FORCE_RADIANS
// #include <glm/gtc/type_ptr.hpp>
#include <algorithm>

/*Material::Material(std::string name)
:
    mName(name)
{

}*/

Material::~Material()
{

}

// *** Public Methods *** //

int Material::sceneIndex(Scene3D* scene) const
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    return (search != end(mScenes)) ? (*search).second : -1;    
}


bool Material::addScene(Scene3D* scene, int index)
{
    if(std::find(begin(mScenes), end(mScenes), std::pair<Scene3D*, int>(scene, index)) == end(mScenes))
    {
        mScenes.push_back({scene, index});
        return true;
    }
    else
        return false;
}

bool Material::removeScene(Scene3D* scene)
{
    auto search = std::find_if(begin(mScenes), end(mScenes), [&scene](std::pair<Scene3D*, int> element){ return (element.first == scene) ? true: false;});
    
    if(search != end(mScenes))
    {
        mScenes.erase(search);
        return true;
    }
    else
        return false;
}

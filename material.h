#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Scene3D;


struct ColorChannel
{
    Texture *mTexture;
    glm::vec4 mColor;

    ColorChannel()
    :
    mTexture(nullptr),
    mColor(0.0, 0.0, 0.0, 1.0)    
    {
    }
    
    ColorChannel(glm::vec4 color)
    :
    mColor(color),
    mTexture(nullptr)
    {        
    }
    
    ColorChannel(Texture *tex)
    :
    mTexture(tex),
    mColor(glm::vec4(1.0))
    {
        
    }
    
    ColorChannel(glm::vec4 color, Texture *tex)
    :
    mColor(color),
    mTexture(tex)
    {
        
    }
};

class Material
{
public:
    Material() : Material(""){};
    Material(std::string name) : Material(name, nullptr){};
    Material(std::string name, Program* shader) : mName(name), mShader(shader){};
    Material(std::string name, ColorChannel diffuse): mName(name), mDiffuse(diffuse){}
    Material(std::string name, ColorChannel diffuse, ColorChannel emissive, ColorChannel specular, ColorChannel ambient, GLfloat shininess):
        mName(name), mDiffuse(diffuse), mEmissive(emissive), mSpecular(specular), mAmbient(ambient), mShinness(shininess){}
    ~Material();
    
    std::string name() const { return mName; }
    Program *shader() { return mShader; }
    GLuint id() { return mDiffuse.mTexture->id(); };
    Texture* diffuseTexture(){ return mDiffuse.mTexture; }
    ColorChannel diffuseChannel() const { return mDiffuse; }
    int sceneIndex(Scene3D* scene) const;
    
    void setShader(Program *shader) { mShader = shader; }
    
    void setEmissiveTexture(Texture *tex){ mEmissive.mTexture = tex; }
    void setAmbientTexture(Texture *tex){ mAmbient.mTexture = tex; }
    void setDiffuseTexture(Texture *tex){ mDiffuse.mTexture = tex; }
    void setSpecularTexture(Texture *tex){ mSpecular.mTexture = tex; }
    
    void setEmissive(ColorChannel emissive) { mEmissive = emissive; };
    void setAmbient(ColorChannel ambient) { mAmbient = ambient; }
    void setDiffuse(ColorChannel diffuse) { mDiffuse = diffuse; }
    void setSpecular(ColorChannel specular) { mSpecular = specular; }
    void setShininess(GLfloat shininess) { mShinness = shininess; }

    bool addScene(Scene3D *scene, int index);
    bool removeScene(Scene3D *scene);    
    
private:    
    std::string mName;
    Program *mShader;
    ColorChannel mEmissive;
    ColorChannel mAmbient;
    ColorChannel mDiffuse;
    ColorChannel mSpecular;
    GLfloat mShinness;
    
    std::vector<std::pair<Scene3D*, int>> mScenes;
};

#endif // MATERIAL_H

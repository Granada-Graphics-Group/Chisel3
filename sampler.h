#ifndef SAMPLER_H
#define SAMPLER_H

#include <GL/glew.h>
#include <unordered_map>
#include <string>

class Sampler
{
public:
    Sampler(std::string name, GLuint unit);
    ~Sampler();

    std::string name() const { return mName; }
    GLuint id() const { return mId; }
    GLuint unit() const { return mTextureUnit; }
    GLint parameterI(GLenum parameter) const;
    GLfloat parameterF(GLenum parameter) const;
    
    void bindUnit(GLuint unit);
    void setParameter(GLenum parameter, GLint value);
    void setParameter(GLenum parameter, GLfloat value);
private:
    std::string mName;
    
    GLuint mId;
    GLuint mTextureUnit;
    std::unordered_map<GLenum, GLint> mParametersI;
    std::unordered_map<GLenum, GLfloat> mParametersF;
};

#endif // SAMPLER_H

#include "sampler.h"

#include <algorithm>

Sampler::Sampler(std::string name, GLuint unit)
:
    mName(name),
    mTextureUnit(unit)
{

}

Sampler::~Sampler()
{

}

//*** Public Methods ***//
GLint Sampler::parameterI(GLenum parameter) const
{
    auto search = mParametersI.find(parameter);
    
    if(search != end(mParametersI))
        return search->second;
    else
        return -1;
}

GLfloat Sampler::parameterF(GLenum parameter) const
{
    auto search = mParametersF.find(parameter);
    
    if(search != end(mParametersF))
        return search->second;
    else
        return -1;
}


void Sampler::bindUnit(GLuint unit)
{
    if(unit > -1) // falta el tope
    {
        mTextureUnit = unit;
        glBindSampler(mTextureUnit, mId);
    }
}

void Sampler::setParameter(GLenum parameter, GLint value)
{
    mParametersI[parameter] = value;
    glSamplerParameteri(mId, parameter, value);
}

void Sampler::setParameter(GLenum parameter, GLfloat value)
{
    mParametersF[parameter] = value;
    glSamplerParameterf(mId, parameter, value);
}

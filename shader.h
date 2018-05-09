#ifndef __SHADER__
#define __SHADER__

#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

class Shader
{
public:
    Shader(GLenum type);
    Shader(std::string name, GLenum type);
    //~Shader();

    std::string name() const;
    void setSourceCode(const std::string& source);
    bool readSourceCode(std::string const &fileName);
    bool compileSourceCode();
    GLuint shaderID() const;
    GLenum shaderType() const { return mShaderType; }
    const std::string& sourceCode() const { return mShaderCode; }
    const std::string& log() const;
    bool isCompiled() const;
    
private:
    std::string mName;
    GLuint mShaderID;
    GLenum mShaderType;
    std::string mShaderCode;
    std::string mCompilationLog;
    bool mIsCompiled;
};

struct Uniform
{
    GLint type;
    GLint arraySize;
    GLint offset;
    GLint location;
    std::vector<GLfloat> data;
};

struct Output
{
    GLint type;
    GLint location;
    GLint fragmentIndex;
};

struct UniformBlock
{
    GLuint index;
    GLint bindingIndex;
    std::map<std::string, Uniform> uniforms;
};



class Program
{
public:
    Program();
    Program(const std::string& name);
    ~Program();
    std::string name() const;
    bool insertShader (const Shader &shader);
    bool linkShaders();
    void detachShaders();
    void deleteShaders();
    bool isLinked() const;
    const std::string& log() const;
    void use();
    void stop();
    GLuint programID() const;

    void bindProgramInputLocation(std::string const &input, GLuint location);
    void bindProgramOutput(std::string const &output, GLuint colorNumber);

    GLuint programInput(std::string const &input);
    GLint uniform(std::string const &uniform);
    const UniformBlock& uniformBlock(const std::string& uniformBlock) const;
    const std::map<std::string, Output>& outputs() const;
    
    GLuint operator[](std::string const &input);
    GLuint operator()(std::string const &uniform);
    const std::vector<GLfloat>& uniformData(const std::string& uniformBlock, const std::string& uniform) const;

    void getProgramInputLocation(std::string const &input);    
    void getUniformLocation(std::string const &uniform);
    
    void getActiveUniformLocations();
    void getActiveProgramInputLocations();
    void getUniformBlocks();
    void getProgramOutputs();
    
    void setUniformData(const std::string& uniformBlock, const std::string& uniform, std::vector<float>& data);
    const std::vector<GLfloat> packUniformData(GLuint bindingIndex) const;
    const std::vector<GLfloat> packUniformData(std::string uniformBlock) const;
    
    void printActiveUniforms() const;
    void printActiveProgramInputs() const;
    
private:
    std::string mName;
    std::vector<Shader> mShaders;
    std::map<std::string, GLuint> mProgInputs;
    std::map<std::string, GLint> mUniforms;
    std::map<std::string, UniformBlock> mUniformBlocks;
    std::map<std::string, Uniform> mUniforms2;
    std::map<std::string, Output> mProgOutputs;

    GLuint mProgramID;
    bool mIsLinked;
    std::string mLog;
};

#endif

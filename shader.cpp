#include "shader.h"
#include "logger.hpp"

#include <fstream>
#include <iostream>
#include <iterator>
#include <array>

using namespace std;

Shader::Shader (GLenum type)
:
    mShaderType(type),
    mIsCompiled(false)
{
    mShaderID = glCreateShader(type);

    if (!mShaderID)
        LOG_ERR("Anonymous shader creation failed");
}

Shader::Shader (string name, GLenum type)
:
    mName(name),
    mShaderType(type),
    mIsCompiled(false)
{
    mShaderID = glCreateShader(type);

    if (!mShaderID)
        LOG_ERR("Shader ", name, ": Creation failed");
}

string Shader::name() const
{
    return mName;
}

void Shader::setSourceCode(const std::string& source)
{
    mShaderCode = source;
}


bool Shader::readSourceCode (string const &fileName)
{
    ifstream file (fileName.c_str(), ios::in);

    if (!file)
    {
        LOG_ERR("Shader ", mName, ": Can't open the source code ", fileName);
        return false;
    }

    mName = fileName.substr(fileName.find_last_of("/") + 1, fileName.npos);

    // longitud del archivo de código
    file.seekg (0, ios::end);
    mShaderCode.resize(file.tellg());
    file.seekg (0, ios::beg);

    // lectura del código
    file.read(&mShaderCode[0], mShaderCode.size());

    // se cierra el archivo
    file.close();

    return true;
}

bool Shader::compileSourceCode()
{
    auto sourceCString = mShaderCode.c_str();
    glShaderSource (mShaderID, 1, &sourceCString, NULL);

    glCompileShader(mShaderID);

    GLint state;
    glGetShaderiv (mShaderID, GL_COMPILE_STATUS, &state);

    if(!state)
    {
        GLint length, written;
        length = written = 0;

        glGetShaderiv (mShaderID, GL_INFO_LOG_LENGTH, &length);

        mCompilationLog.resize(length);

        glGetShaderInfoLog (mShaderID, length, &written, &mCompilationLog[0]);

        return false;
    }

    mIsCompiled = true;
    return mIsCompiled;
}

GLuint Shader::shaderID() const
{
    return mShaderID;
}

const std::string& Shader::log() const
{
    return mCompilationLog;
}

bool Shader::isCompiled() const
{
    return mIsCompiled;
}


Program::Program()
:
    mIsLinked(false)
{
    mProgramID = glCreateProgram();
    
    if(!mProgramID)
        LOG("Anonymous program creation failed");
}

Program::~Program()
{
    LOG("Destroying Program: ", mName);
    detachShaders();
    deleteShaders();
    
    glDeleteProgram(mProgramID);
}


Program::Program(const std::string& name)
:
    mName(name),
    mIsLinked(false)
{
    mProgramID = glCreateProgram();
    
    if(!mProgramID)
        LOG("Program ", mName, ": Creation failed");
}


string Program::name() const
{
    return mName;
}

bool Program::insertShader(const Shader& shader)
{
    if (shader.isCompiled())
    {
        mShaders.push_back(shader);
        return true;
    }
    else
    {
        LOG_WARN("Program ", mName, ": The shader ", shader.name(), " didn't compile");
        return false;
    }
}

bool Program::linkShaders()
{
    vector<Shader>::const_iterator iterator = this->mShaders.begin();

    while (iterator != this->mShaders.end())
    {
        LOG("Attaching shader ", iterator->name());
        glAttachShader (mProgramID, iterator->shaderID());
        iterator++;
    }

    glLinkProgram (mProgramID);

    GLint linked;

    glGetProgramiv(mProgramID, GL_LINK_STATUS, &linked);

    if(!linked)
    {
        GLint length, written;
        length = written = 0;

        glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH,&length);

        mLog.resize(length);

        glGetProgramInfoLog(mProgramID, length, &written, &mLog[0]);

        return false;
    }
    
    mIsLinked = true;
    
    return mIsLinked;
}

void Program::detachShaders()
{
    for(const auto& shader : mShaders)
        glDetachShader(mProgramID, shader.shaderID());
}

void Program::deleteShaders()
{
    for(const auto& shader : mShaders)
        glDeleteShader(shader.shaderID());
    
    mShaders.clear();
}


bool Program::isLinked() const
{
    return mIsLinked;
}

const std::string& Program::log() const
{
    return mLog;
}

void Program::use()
{
    glUseProgram(mProgramID);
}

void Program::stop()
{
    glUseProgram(0);
}

GLuint Program::programID() const
{
    return this->mProgramID;
}

void Program::getProgramInputLocation(const string& input)
{
    mProgInputs[input] = glGetAttribLocation(mProgramID, input.c_str());
}

void Program::bindProgramInputLocation(const string& input, GLuint location)
{
    glBindAttribLocation(mProgramID, location, input.c_str());
    mProgInputs[input] = location;
}

void Program::getUniformLocation(string const &uniform)
{
    mUniforms[uniform] = glGetUniformLocation(mProgramID, uniform.c_str());
}

GLuint Program::programInput(const string& input)
{
    return mProgInputs[input];
}

GLint Program::uniform(const string& uniform)
{    
    if(mUniforms.find(uniform) != end(mUniforms))
        return mUniforms[uniform];
    else
        return -1;
}

const UniformBlock& Program::uniformBlock(const string& uniformBlock) const
{
    return mUniformBlocks.at(uniformBlock);
}

const map< string, Output >& Program::outputs() const
{
    return mProgOutputs;
}


GLuint Program::operator[](const string& input)
{
    return mProgInputs[input];
}

GLuint Program::operator()(string const &uniform)
{
    if(mUniforms.find(uniform) != end(mUniforms))
        return mUniforms[uniform];
    else
        return -1;
}

const vector< GLfloat >& Program::uniformData(const string& uniformBlock, const string& uniform) const
{
    return mUniformBlocks.at(uniformBlock).uniforms.at(uniform).data;
}


void Program::bindProgramOutput(string const &output, GLuint colorNumber)
{
    glBindFragDataLocation(mProgramID, colorNumber, output.c_str());
    mProgOutputs[output].location = colorNumber;
}

void Program::getActiveUniformLocations()
{
    GLint numUniforms, maxLength;

    glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

    auto name = std::make_unique<GLchar[]>(maxLength);
    GLsizei length;

    for(int i = 0; i < numUniforms; ++i)
    {
        glGetActiveUniformName(mProgramID, i, maxLength, &length, name.get());        
        mUniforms[string(name.get())] = glGetUniformLocation(mProgramID, name.get());
        LOG("Active uniform: ", string(name.get()), " l: ", mUniforms[string(name.get())]);
    }
}

void Program::getActiveProgramInputLocations()
{
    GLint numAttributes, maxLength;

    glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

    auto name = std::make_unique<GLchar[]>(maxLength);
    GLsizei length;
    GLint size;
    GLenum type;

    for(int i = 0; i < numAttributes; ++i)
    {
        glGetActiveAttrib(mProgramID, i, maxLength, &length, &size, &type, name.get());
        mProgInputs[string(name.get())] = glGetAttribLocation(mProgramID, name.get());
    }
}

void Program::getUniformBlocks()
{
    GLint numBlocks;
    const std::array<GLenum, 3> blockProperties = { GL_NUM_ACTIVE_VARIABLES, GL_NAME_LENGTH, GL_BUFFER_BINDING };
    const std::array<GLenum, 1> activeUnifProp = { GL_ACTIVE_VARIABLES };
    const std::array<GLenum, 5> unifProperties = { GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_LOCATION };
    
    glGetProgramInterfaceiv(mProgramID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks );
    
    for(int blockIndex = 0; blockIndex < numBlocks; ++blockIndex)
    {
        UniformBlock block;        
        std::array <GLint, 3> blockQuery;
        
        glGetProgramResourceiv(mProgramID, GL_UNIFORM_BLOCK, blockIndex, 3, blockProperties.data(), 3, NULL, blockQuery.data());
        
        std::vector<char> blockNameData(blockQuery[1] + 1);
        
        glGetProgramResourceName(mProgramID, GL_UNIFORM_BLOCK, blockIndex, blockNameData.size(), NULL, &blockNameData[0]);
        std::string blockName(blockNameData.data());        
        
        if(!blockQuery[0])
            continue;        
        
        std::vector<GLint> blockUnifs(blockQuery[0]);        
        glGetProgramResourceiv(mProgramID, GL_UNIFORM_BLOCK, blockIndex, 1, activeUnifProp.data(), blockQuery[0], NULL, &blockUnifs[0]);
        
        block.index = blockIndex;
        block.bindingIndex = blockQuery[2];
        mUniformBlocks[blockName] = std::move(block);

        
        for(int uniformIndex = 0; uniformIndex < blockQuery[0]; ++uniformIndex)
        {
            std::array<GLint, 5> values;
            
            glGetProgramResourceiv(mProgramID, GL_UNIFORM, blockUnifs[uniformIndex], 5, unifProperties.data(), 5, NULL, values.data());
            
            std::vector<char> nameData(values[0] + 1);
            glGetProgramResourceName(mProgramID, GL_UNIFORM, blockUnifs[uniformIndex], nameData.size(), NULL, &nameData[0]);

            std::string uniformName(nameData.data());            
            mUniformBlocks[blockName].uniforms[uniformName] = Uniform{values[1], values[2], values[3], values[4]};
            
            //LOG("-> uniform: ", uniformName, " type: ", values[1], " ArraySize: ", values[2], " offset: ", values[3], " location: ", values[4]);                        
        }
        
//         for(auto uniform : mUniformBlocks[blockName].uniforms)
//             LOG("<> uniform: ", uniform.first, " ", uniform.second.type, " ", uniform.second.location);
//         LOG("End");
    }
}

void Program::getProgramOutputs()
{
    GLint numOutputs;
    const std::array<GLenum, 4> outputProperties = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_LOCATION_INDEX };

    glGetProgramInterfaceiv(mProgramID, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &numOutputs );
    
    for(int outputIndex = 0; outputIndex < numOutputs; ++outputIndex)
    {
        Output output;        
        std::array <GLint, 4> outputQuery;
        
        glGetProgramResourceiv(mProgramID, GL_PROGRAM_OUTPUT, outputIndex, 4, outputProperties.data(), 4, NULL, outputQuery.data());
        
        std::vector<char> outputNameData(outputQuery[1] + 1);
        
        glGetProgramResourceName(mProgramID, GL_PROGRAM_OUTPUT, outputIndex, outputNameData.size(), NULL, &outputNameData[0]);
        std::string outputName(outputNameData.data());        
        
        if(!outputQuery[0])
            continue;
        
        output.type = outputQuery[1];
        output.location = outputQuery[2];
        output.fragmentIndex = outputQuery[3];
        
        mProgOutputs[outputName] = std::move(output);
    }
}


void Program::setUniformData(const string& uniformBlock, const string& uniform, vector< float >& data)
{
    mUniformBlocks[uniformBlock].uniforms[uniform].data = data;
}

const vector< GLfloat > Program::packUniformData(GLuint bindingIndex) const
{
    vector<GLfloat> data;
    
    for(auto uniformBlock: mUniformBlocks)
        if(uniformBlock.second.bindingIndex == bindingIndex)            
            for(auto uniform: uniformBlock.second.uniforms)
            {
                const auto& uniformData = uniform.second.data;
              
                std::copy(begin(uniformData), end(uniformData), std::back_inserter(data));
                
                if(uniformData.size() > 1)
                    data.insert(end(data), 4 - uniformData.size(), 0.0);                
            }
            
    return data;
}   



void Program::printActiveUniforms() const
{
    LOG("Listing active uniforms: ");
    for(auto uniform: mUniforms)
        LOG("-- ", uniform.first, ": ", uniform.second);
    
    LOG("Listing active uniform Blocks: ");
    for(auto uniformBlock : mUniformBlocks)
    {
        LOG("-- ", uniformBlock.first, ": ", uniformBlock.second.index);
        
        for(auto uniform : uniformBlock.second.uniforms)
            LOG("  - ", uniform.first, ": ", uniform.second.location);
    }
}

void Program::printActiveProgramInputs() const
{
    LOG("Listing active programs inputs: ");
    for(auto input: mProgInputs)
        LOG(input.first, ": ", input.second);    
    //for(std::map <string, GLuint>::const_iterator iter = mProgInputs.begin(); iter != mProgInputs.end(); ++iter)
    //    std::cout << iter->first << ": " << iter->second << std::endl;    
}

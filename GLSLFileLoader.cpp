/***************************************************************************
 *   Copyright (C) 2007 by Luis   *
 *   azelsestren@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <iterator>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <QDebug>
#include <QApplication>
#include <pugixml.hpp>

#include "GLSLFileLoader.h"
#include "logger.hpp"

using namespace std;

GLSLFileLoader::GLSLFileLoader (QString& path, QStringList& filters, std::map< string, Shader* >& shaders, std::map< string, std::unique_ptr<Program> >& programs)
    :
    mDir(path),
    mFilters(filters),
    mShaders(shaders),
    mPrograms(programs)
{
    mExtensionToShaderType["vert"] = GL_VERTEX_SHADER;
    mExtensionToShaderType["frag"] = GL_FRAGMENT_SHADER;
    mExtensionToShaderType["geom"] = GL_GEOMETRY_SHADER;
    mExtensionToShaderType["tessc"] = GL_TESS_CONTROL_SHADER;
    mExtensionToShaderType["tesse"] = GL_TESS_EVALUATION_SHADER;
    mExtensionToShaderType["comp"] = GL_COMPUTE_SHADER;

    mFileTypeToShaderType["vertex"] = GL_VERTEX_SHADER;
    mFileTypeToShaderType["fragment"] = GL_FRAGMENT_SHADER;
    mFileTypeToShaderType["geometry"] = GL_GEOMETRY_SHADER;
    mFileTypeToShaderType["tessControl"] = GL_TESS_CONTROL_SHADER;
    mFileTypeToShaderType["tessEval"] = GL_TESS_EVALUATION_SHADER;
    mFileTypeToShaderType["compute"] = GL_COMPUTE_SHADER;
}

void GLSLFileLoader::parseGLSLEffectFiles()
{
    QFileInfoList fileList = mDir.entryInfoList(mFilters, QDir::Files);
    
    pugi::xml_document document;
    auto result = document.load_file(fileList.at(0).absoluteFilePath().toStdString().c_str());
    
    if(result.status == pugi::status_ok)
    {    
        auto programs = document.child("programs");
        for(auto program : programs.children("program"))
        {
            std::string programName = program.attribute("name").as_string();
            auto currentProgram = std::make_unique<Program>(programName);            
            
            LOG("Loading Program shader ", currentProgram->name());
            
            for(auto shader: program.child("shaders").children("shader"))
            {
                std::string shaderName = shader.attribute("name").as_string();
                std::string shaderType = shader.attribute("type").as_string();
                
                // Comprobar que no existe previamente
                Shader currentShader(shaderName, mFileTypeToShaderType[shaderType]);
                //mShaders[shaderName] = currentShader;
                
                currentShader.readSourceCode(mDir.absoluteFilePath(QString(currentShader.name().c_str())).toStdString());
                currentShader.compileSourceCode();

                if(!currentShader.isCompiled())
                    LOG_ERR(currentShader.name(), ":\n", currentShader.log());
                
                currentProgram->insertShader(currentShader);
            }
                        
            currentProgram->linkShaders();

            if (!currentProgram->isLinked())
                LOG_ERR(currentProgram->log());
            else
            {
                currentProgram->detachShaders();
                // Borrado deberia ser al final porque distintos programas pueden compartir el shader                
                currentProgram->deleteShaders();
            }
                    
            currentProgram->getUniformBlocks();
            currentProgram->getProgramOutputs();
            currentProgram->getActiveProgramInputLocations();
            currentProgram->getActiveUniformLocations();
            
            currentProgram->printActiveUniforms();
            currentProgram->printActiveProgramInputs();
            
            auto uniforms = program.child("uniforms");
            auto uniformBlocks = program.child("uniformBlocks");
            
            for(auto uniform: uniforms.children("uniform"))
            {
                std::string uniformName = uniform.attribute("name").as_string();
                LOG("uniformName: ", uniformName);
            }
            
            for(auto uniformBlock: uniformBlocks.children("uniformBlock"))
            {
                std::string uniformBlockName = uniformBlock.attribute("name").as_string();
                
                for(auto uniform: uniformBlock.children("uniform"))
                {                       
                    std::string uniformName = uniform.attribute("name").as_string();
                    std::string uniformValue = uniform.attribute("value").as_string();
                    
                    std::vector<GLfloat> uniformData = parseUniformValue(uniformValue);
                    
                    currentProgram->setUniformData(uniformBlockName, uniformName, uniformData);
                    
                    const auto & data = currentProgram->uniformData(uniformBlockName, uniformName);                    
                    
                    LOG("Program: ", currentProgram->name(), " uBlock ", uniformBlockName, ", u ", uniformName, ", d: ", data[0], " ", data[1], " ", data[2] );
                }
            }

            mPrograms[programName] = std::move(currentProgram);
        }
    }
    
//     Program* program = mPrograms["Blend3"];
//     
//     auto data = program->uniformData("colorVec", "red");
//     
//     LOG("red size: ", data.size());
//     
//     LOG("RED: ", data.at(0), " ", data.at(1), " ", data.at(2));
    
    
    
/*    ifstream glslFile;
    string line;
    unsigned int lineNumber = 0;
    vector<string> words;
    
    for(int i = 0; i < fileList.size(); ++i)
    {
        string fileName = fileList.at(i).absoluteFilePath().toStdString();

        glslFile.open(fileName.c_str(), ios::in);

        Program * currentProgram = 0;

        while(!glslFile.eof())
        {
            lineNumber++;
            extraerPalabrasLineaSiguiente(glslFile, line, words, " ");

            if(!words[0].compare("Program"))
            {
                if (currentProgram != 0)
                {
                    LOG("Loading Program shader ", currentProgram->name());

                    currentProgram->linkShaders();

                    if (!currentProgram->isLinked())
                        LOG_ERR(currentProgram->log());
                    
                    currentProgram->getUniformBlocks();
                    currentProgram->getActiveProgramInputLocations();
                    currentProgram->getActiveUniformLocations();

                    currentProgram->printActiveUniforms();

                    currentProgram->printActiveProgramInputs();
                    
                    currentProgram->bindProgramOutput("FragColor", 0);
                }
                
                currentProgram = new Program(words[1]);
                mPrograms[words[1]] = currentProgram;
            }
            else
            {
                bool validShader = true;

                Shader *currentShader = 0;

                if(words.size() < 2)
                {
                    string extension = words[0].substr(words[0].find_last_of(".") + 1, words[0].npos);

                    if (extension.size() and mExtensionToShaderType.count(extension) > 0)
                    {
                        currentShader = new Shader(words[0], mExtensionToShaderType[extension]);
                        mShaders[words[0]] = currentShader;
                    }
                    else
                        validShader = false;
                }
                else
                {
                    if(mFileTypeToShaderType.count(words[0]) > 0)
                    {
                        currentShader = new Shader(words[1], mFileTypeToShaderType[words[0]]);
                        mShaders[words[1]] = currentShader;
                    }
                    else
                        validShader = false;
                }

                if(validShader)
                {
                    currentShader->readSourceCode(mDir.absoluteFilePath(QString(currentShader->name().c_str())).toStdString());
                    currentShader->compileSourceCode();

                    if(!currentShader->isCompiled())
                        LOG_ERR(currentShader->name(), ":\n", currentShader->log());

                    currentProgram->insertShader(*currentShader);
                }
                else
                    LOG_ERR("Error in line[", lineNumber, "]: \"", line, "\" : Shader type is not valid");
            }
        }

        LOG("Loading Program shader ", currentProgram->name());

        currentProgram->linkShaders();

        if (!currentProgram->isLinked())
            currentProgram->log();

        currentProgram->getUniformBlocks();
        
        currentProgram->getActiveProgramInputLocations();
        currentProgram->getActiveUniformLocations();

        currentProgram->printActiveUniforms();

        currentProgram->printActiveProgramInputs();
    }*/
    
}

//*** Private Methods ***//

std::vector< GLfloat > GLSLFileLoader::parseUniformValue(std::string& valueField)
{
    std::vector<GLfloat> values;
    std::vector<string> words;
    
    extraerPalabras(words, valueField, " ");
    
    for(auto word: words)
        values.push_back(stof(word));
    
    return values;
}



void GLSLFileLoader::extraerPalabras (vector< string >& words, string& line, const string& delimiter)
{
    size_t pos, posAnterior = 0;
    words.clear();

    while ( ( pos = line.find ( delimiter, posAnterior ) ) != string::npos ) {
        words.push_back ( line.substr ( posAnterior,pos-posAnterior ) );

        posAnterior = pos + delimiter.size();
    }

    words.push_back ( line.substr ( posAnterior, line.size() ) );
}

void GLSLFileLoader::extraerPalabrasLineaSiguiente (ifstream& file, vector< string >& words, const string& delimiter)
{
    string line;
    size_t pos, lastPos = 0;

    words.clear();

    getline ( file, line );

    while ( ( pos = line.find ( delimiter, lastPos ) ) != string::npos )
    {
        if(pos != lastPos)
            words.push_back ( line.substr ( lastPos,pos-lastPos ) );

        lastPos = pos + delimiter.size();
    }

    words.push_back ( line.substr ( lastPos, line.size() ) );
}

void GLSLFileLoader::extraerPalabrasLineaSiguiente (ifstream& file, string& line, vector< string >& words, const string& delimiter)
{
    size_t pos, lastPos = 0;

    words.clear();

    getline ( file, line );

    while ( ( pos = line.find ( delimiter, lastPos ) ) != string::npos )
    {
        if(pos != lastPos)
            words.push_back ( line.substr ( lastPos,pos-lastPos ) );

        lastPos = pos + delimiter.size();
    }

    words.push_back ( line.substr ( lastPos, line.size() ) );
}


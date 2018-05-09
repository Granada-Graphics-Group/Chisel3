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
#ifndef _GLSLFILELOADER__
#define _GLSLFILELOADER__

#include "shader.h"

#include <memory>
#include <vector>
#include <set>
#include <fstream>
#include <map>
#include <QDir>

class GLSLFileLoader
{
public:
    GLSLFileLoader(QString &path, QStringList &filters, std::map< std::string, Shader* >& shaders, std::map< std::string, std::unique_ptr<Program> >& programs);

    void parseGLSLEffectFiles();
private:
    std::vector<GLfloat> parseUniformValue(std::string& valueField);
    void extraerPalabras(std::vector<std::string> &words, std::string &line, std::string const &delimiter);
	void extraerPalabrasLineaSiguiente(std::ifstream &file, std::vector<std::string> &words, std::string const&delimiter);
    void extraerPalabrasLineaSiguiente(std::ifstream& file, std::string &line, std::vector< std::string >& words, const std::string& delimiter);

    QDir mDir;
    QStringList mFilters;
    std::map<std::string, Shader *> &mShaders;
    std::map<std::string, std::unique_ptr<Program>> &mPrograms;
    std::map<std::string, GLenum> mExtensionToShaderType;
    std::map<std::string, GLenum> mFileTypeToShaderType;
};

#endif

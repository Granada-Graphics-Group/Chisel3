#ifndef COMPUTETECH_H
#define COMPUTETECH_H

#include <GL/glew.h>
#include <string>
#include <array>

class Program;

class ComputeTech
{
    ComputeTech(std::string name, Program* shader);

    Program* shader() { return mShader; }
    const std::array<GLuint, 3>& groupCount() { return mGroupCount; }
    const std::array<GLuint, 3>& groupSize() { return mGroupSize; }

private:
    std::string mName;
    Program* mShader;
    std::array<GLuint, 3> mGroupCount;
    std::array<GLuint, 3> mGroupSize;
};

#endif
#ifndef COMPUTEJOB_H
#define COMPUTEJOB_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/raw_data.hpp>
#include <vector>
#include <string>
#include <array>

class Program;

class ComputeJob
{
public:    
    ComputeJob(std::string name, Program* shader) : ComputeJob(name, shader, {0, 0, 0}, {0, 0, 0}){};
    ComputeJob(std::string name, Program* shader, const std::array<GLuint, 3>& groupCount, const std::array<GLuint, 3>& groupSize);

    Program* shader() const { return mShader; }
    const std::array<GLuint, 3>& groupCount() const { return mGroupCount; }
    const std::array<GLuint, 3>& groupSize() const { return mGroupSize; }
    const std::vector<GLint>& indices()  const { return mIndices; }
    const std::vector<glm::byte> data() const { return mData; }
    GLbitfield memoryBarriers() const { return mBarriers; }
    
    void setIndices(const std::vector<int>& indices){ mIndices = indices; }
    void updateData(GLintptr offset, GLsizeiptr size, const GLvoid* data);
    void setMemoryBarriers(GLbitfield barriers) { mBarriers = barriers; }
    void addMemoryBarrier(GLbitfield barrier) { mBarriers |= barrier; }
    void delMemoryBarrier(GLbitfield barrier) { mBarriers &= ~barrier; }

private:
    std::string mName;
    Program* mShader;    
    std::array<GLuint, 3> mGroupCount;
    std::array<GLuint, 3> mGroupSize;
    std::vector<GLint> mIndices;
    std::vector<glm::byte> mData;
    GLbitfield mBarriers;
};

#endif


#include "computejob.h"

ComputeJob::ComputeJob(std::string name, Program* shader, const std::array<GLuint, 3>& groupCount, const std::array<GLuint, 3>& groupSize)
:
    mName(name),
    mShader(shader),
    mGroupCount(groupCount),
    mGroupSize(groupSize)
{
    
}


// *** Public Methods *** //

void ComputeJob::updateData(GLintptr offset, GLsizeiptr size, const GLvoid* data)
{
    const glm::byte *byteData = reinterpret_cast<const glm::byte *>(data);
        
    if((size + offset) > mData.size())
        mData.resize(size + offset);
        
    std::copy(byteData, byteData + size, begin(mData) + offset);    
}

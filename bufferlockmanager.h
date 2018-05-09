#ifndef BUFFERLOCKMANAGER_H
#define BUFFERLOCKMANAGER_H

#include "GL/glew.h"
#include "vector"

struct BufferRange
{
    size_t mStartOffset;
    size_t mLength;

    bool Overlaps(const BufferRange& _rhs) const {
        return mStartOffset < (_rhs.mStartOffset + _rhs.mLength)
            && _rhs.mStartOffset < (mStartOffset + mLength);
    }
};


struct BufferLock
{
    BufferRange mRange;
    GLsync mSyncObj;
};


class BufferLockManager
{
public:
    BufferLockManager(bool cpuUpdates);
    ~BufferLockManager();

    void WaitForLockedRange(size_t lockBeginBytes, size_t lockLength);
    void LockRange(size_t lockBeginBytes, size_t lockLength);

private:
    void wait(GLsync* syncObj);
    void cleanup(BufferLock* bufferLock);

    std::vector<BufferLock> mBufferLocks;

    // Whether it's the CPU (true) that updates, or the GPU (false)
    bool mCPUUpdates;
};

#endif // BUFFERLOCKMANAGER_H

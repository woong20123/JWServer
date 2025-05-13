#include "PacketBufferPool.h"
#include "PacketBuffer.h"

namespace jw
{
    PacketBufferPool::PacketBufferPool()
    {}

    PacketBufferPool::Ptr PacketBufferPool::Acquire()
    {
        PacketBuffer* ret{ _objectPool.Acquire() };
        ++_useCount;
        return ret;
    }

    void PacketBufferPool::Release(PacketBufferPool::Ptr obj)
    {
        if (!obj) return;
        _objectPool.Release(obj);
        --_useCount;
    }

    uint64_t PacketBufferPool::GetUseCount()
    {
        uint64_t useCount = _useCount;
        return useCount;
    }
}
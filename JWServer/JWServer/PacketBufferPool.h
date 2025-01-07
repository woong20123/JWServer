#pragma once
#ifndef __JW_LOG_PACKET_BUFFER_POOL_H__
#define __JW_LOG_PACKET_BUFFER_POOL_H__
#include <memory>
#include "Singleton.hpp"
#include "ObjectPool.hpp"

namespace jw
{
    class PacketBuffer;
    class PacketBufferPool : public Singleton<PacketBufferPool>
    {
    public:
        static constexpr size_t BASE_ALLOCATE_COUNT = 128;
        static constexpr size_t MAX_ALLOCATE_COUNT = 32768;

        PacketBuffer* Acquire();
        void Release(PacketBuffer* obj);
        uint64_t GetUseCount();
    protected:
        PacketBufferPool();
    private:
        std::atomic<uint64_t>                                                   _useCount;
        ObjectPool<PacketBuffer, BASE_ALLOCATE_COUNT, MAX_ALLOCATE_COUNT>       _objectPool;
        friend class Singleton<PacketBufferPool>;

    };
}

#define PACKET_BUFFER_POOL jw::PacketBufferPool::GetInstance

#endif


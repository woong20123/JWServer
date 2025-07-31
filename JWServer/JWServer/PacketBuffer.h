#pragma once
#ifndef __JW_PACKET_BUFFER_H__
#define __JW_PACKET_BUFFER_H__
#include <cstdint>

namespace jw
{
    class PacketBuffer
    {
    public:
        PacketBuffer();
        ~PacketBuffer();
        static constexpr const size_t BUFFER_SIZE = 4000;
        using BufferType = char;

        void* GetBuffer();

        void Initialize();

    private:
        void clearBuffer();

        uint16_t        _setBufferSize;
        BufferType      _buffer[BUFFER_SIZE];
    };
}
#endif // !__JW_PACKET_BUFFER_H__


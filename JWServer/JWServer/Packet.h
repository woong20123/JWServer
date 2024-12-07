#pragma once
#ifndef __JW_PACKET_H__
#define __JW_PACKET_H__
#include <cstdint>

namespace jw
{
    class Packet
    {
    public:
        Packet();

        using packetSize = uint16_t;
        struct Header {
            packetSize  _size{ 0 };
        };
        static constexpr packetSize	MAX_SIZE = 4000;
        static constexpr packetSize	HEADER_SIZE = sizeof(Header);

        void Add(const void* data, packetSize size);

        bool IsSet() const;
        void SetBuffer(void* buffer);
        Header* GetHeader() const;
        char* GetBody() const;
    private:
        packetSize getSize();
        packetSize getFreeSize();
        Header* _header;
    };
}
#endif


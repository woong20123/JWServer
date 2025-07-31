#pragma once
#ifndef __JW_PACKET_H__
#define __JW_PACKET_H__
#include <cstdint>
#include <memory>

namespace jw
{
    class PacketBuffer;
    struct Impl;
    class Packet
    {
    public:
        Packet();
        ~Packet();

        using packetSize = uint16_t;
        struct Header {
            packetSize  _size{ 0 };
        };
        static constexpr packetSize	HEADER_SIZE = sizeof(Header);

        void Allocate();

        bool Add(const void* data, packetSize size);

        bool IsSet() const;
        void SetBuffer(void* buffer);
        Header* GetHeader() const;
        char* GetBody() const;
        packetSize GetBodySize() const;
        packetSize GetTotalSize() const;
    private:

        packetSize getSize() const;
        packetSize getFreeSize() const;

        std::unique_ptr<Impl> _impl;
    };
}
#endif


#pragma once
#ifndef __JW_PACKET_H__
#define __JW_PACKET_H__
#include <cstdint>
#include <memory>

namespace jw
{
    class PacketBuffer;
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

        bool Add(const void* data, packetSize size);

        bool IsSet() const;
        void SetBuffer(void* buffer);
        void SetPacketBuffer(std::shared_ptr<PacketBuffer>& buffer);
        Header* GetHeader() const;
        char* GetBody() const;
        packetSize GetBodySize() const;
        packetSize GetTotalSize() const;
    private:
        packetSize getSize() const;
        packetSize getFreeSize() const;
        Header* _header;

        std::shared_ptr<PacketBuffer> _packetBuffer;
    };
}
#endif


#pragma once
#ifndef __JW_PACKET_BUFFER_HANDLER_H__
#define __JW_PACKET_BUFFER_HANDLER_H__
#include <cstdint>

namespace jw
{
    class	Packet
    {
    public:
        using packetSize = uint16_t;
        struct Header {
            packetSize _size;
        };
        static constexpr packetSize	MAX_SIZE = 4000;
        static constexpr packetSize	HEADER_SIZE = sizeof(Header);
    };

    struct ToPacketInfo
    {
        bool _isError;
        uint32_t _packetSize;
        void* _packetPointer;
    };

    class PacketBufferHandler
    {
    public:
        using packetSize = uint16_t;

        virtual packetSize PacketHaederSize() = 0;
        virtual ToPacketInfo EnableToPacket(const char* buffer, packetSize bufferSize) = 0;
    };


    class TrustedPacketBufferHandler : public PacketBufferHandler
    {
    public:
        packetSize PacketHaederSize() override;
        ToPacketInfo EnableToPacket(const char* buffer, packetSize bufferSize) override;
    };
}


#endif

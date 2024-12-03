#pragma once
#ifndef __JW_PACKET_BUFFER_HANDLER_H__
#define __JW_PACKET_BUFFER_HANDLER_H__
#include <cstdint>

namespace jw
{
    struct PacketBufferInfo
    {
        bool _isError{ false };
        uint32_t _packetSize{ 0 };
        void* _packetBuffer{ nullptr };
    };

    class PacketBufferHandler
    {
    public:
        using packetSize = uint16_t;

        virtual packetSize PacketHaederSize() = 0;
        virtual PacketBufferInfo EnableToPacket(const char* buffer, packetSize bufferSize) = 0;
    };


    class TrustedPacketBufferHandler : public PacketBufferHandler
    {
    public:
        packetSize PacketHaederSize() override;
        PacketBufferInfo EnableToPacket(const char* buffer, packetSize bufferSize) override;
    };
}


#endif

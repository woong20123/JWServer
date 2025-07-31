#include "PacketBufferHandler.h"
#include "Packet.h"
#include "PacketBuffer.h"
#include "Logger.h"

namespace jw
{
    PacketBufferHandler::packetSize TrustedPacketBufferHandler::PacketHaederSize()
    {
        return Packet::HEADER_SIZE;
    }

    PacketBufferInfo TrustedPacketBufferHandler::EnableToPacket(const char* buffer, PacketBufferHandler::packetSize bufferSize)
    {
        PacketBufferInfo result;

        if (bufferSize < Packet::HEADER_SIZE)
            return result;

        Packet::Header* header = (Packet::Header*)buffer;
        const auto packetSize = header->_size;
        result._packetSize = packetSize;

        if (packetSize < Packet::HEADER_SIZE || PacketBuffer::BUFFER_SIZE < packetSize)
        {
            LOG_FETAL(L"Invalid Packet Size, packetSize:{}", packetSize);
            //result._isError = true;
            return result;
        }

        if (bufferSize < packetSize)
            return result;

        result._packetBuffer = header;
        return result;
    }
}

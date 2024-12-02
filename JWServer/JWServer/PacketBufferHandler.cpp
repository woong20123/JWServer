#include "PacketBufferHandler.h"
#include "Logger.h"

namespace jw
{
    PacketBufferHandler::packetSize TrustedPacketBufferHandler::PacketHaederSize()
    {
        return Packet::HEADER_SIZE;
    }

    ToPacketInfo TrustedPacketBufferHandler::EnableToPacket(const char* buffer, PacketBufferHandler::packetSize bufferSize)
    {
        ToPacketInfo result{ false, 0, nullptr };

        if (bufferSize < Packet::HEADER_SIZE)
            return result;

        Packet::Header* header = (Packet::Header*)buffer;
        const auto packetSize = header->_size;
        result._packetSize = packetSize;

        if (packetSize < Packet::HEADER_SIZE || Packet::MAX_SIZE < packetSize)
        {
            LOG_FETAL(L"Invalid Packet Size, packetSize:{}", packetSize);
            result._isError = true;
            return result;
        }

        if (bufferSize < packetSize)
            return result;

        result._packetPointer = header;
        return result;
    }
}

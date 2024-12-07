#pragma once
#ifndef __JW_PACKET_HANDELR_H__
#define __JW_PACKET_HANDELR_H__
#include <functional>
#include <unordered_map>

namespace jw
{
    class Session;
    class Packet;
    class PacketHandler
    {
    public:
        using cmdType = uint32_t;
        using HandlerType = std::function<bool(const Session*, const Packet&)>;

        PacketHandler() {};
        virtual ~PacketHandler() {};

        virtual bool RegistHandler(const cmdType cmd, HandlerType handler) = 0;
        virtual bool HandlePacket(const Session* session, const  Packet& packet) = 0;
    };
}

#endif // !__JW_PACKET_HANDELR_H__


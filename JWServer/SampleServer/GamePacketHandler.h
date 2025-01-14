#pragma once
#ifndef __JW_GAME_PACKET_HANDLER_H__
#define __JW_GAME_PACKET_HANDLER_H__
#include "PacketHandler.h"

namespace jw
{
    class GamePacketHandler : public PacketHandler
    {
    public:
        bool RegistHandler(const cmdType cmd, const HandlerType handler) override;
        bool HandlePacket(Session* session, const Packet& packet) override;
    private:
        std::unordered_map<cmdType, HandlerType>    _packetHandlersMap;
    };
}

#endif // !__JW_GAME_PACKET_HANDLER_H__


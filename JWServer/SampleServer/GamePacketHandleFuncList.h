#pragma once
#ifndef __JW_GAME_PACKET_HANDLE_FUNC_LIST_H__
#define __JW_GAME_PACKET_HANDLE_FUNC_LIST_H__
#include "Session.h"
#include "Packet.h"
#include "Logger.h"
#include "Singleton.hpp"

namespace jw
{
    class PacketHandler;
    class GamePacketHandleFuncList : public Singleton<GamePacketHandleFuncList>
    {
    public:
        void Initialize(std::shared_ptr<PacketHandler>& packetHandler);
        bool HandleGamePingReq(Session* session, const Packet& packet);
        bool HandleGameLoginReq(Session* session, const Packet& packet);
        bool HandleGameCreateRoomReq(Session* session, const Packet& packet);
        bool HandleGameRoomListReq(Session* session, const Packet& packet);
        bool HandleGameRoomEnterReq(Session* session, const Packet& packet);
        bool HandleGameChatReq(Session* session, const Packet& packet);
    protected:
        GamePacketHandleFuncList() = default;
        ~GamePacketHandleFuncList() = default;
        GamePacketHandleFuncList(GamePacketHandleFuncList&) = delete;
        GamePacketHandleFuncList& operator=(const GamePacketHandleFuncList&) = delete;
    private:
        void registGamePacketHandleList();
        static void* getPacketData(const Packet& packet);
        std::shared_ptr<PacketHandler> _packetHandler;

        friend class Singleton<GamePacketHandleFuncList>;
    };
}

#define GAME_PACKET_HANDLE_FUNC_LIST jw::GamePacketHandleFuncList::GetInstance

#endif


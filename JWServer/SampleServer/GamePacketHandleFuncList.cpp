#include "GamePacketHandleFuncList.h"
#include "PacketHandler.h"
#include "../Packet/GamePacket/GamaPacket.pb.h"

#define REGIST_HANDLE(cmd, handleFun) _packetHandler->RegistHandler(cmd, std::bind(&handleFun, this, std::placeholders::_1, std::placeholders::_2));

#define PARSER_PROTO_PACKET_DATA(protoPacket, req, packetData) \
protoPacket req; \
if (!req.ParseFromArray(packetData, static_cast<int>(req.ByteSizeLong()))){ LOG_ERROR(L"fail req parser, sessionId:{}", session->GetId()); return false; }

namespace jw
{
    void GamePacketHandleFuncList::Initialize(std::shared_ptr<PacketHandler>& packetHandler)
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        _packetHandler = packetHandler;
        registGamePacketHandleList();
    }

    void GamePacketHandleFuncList::registGamePacketHandleList()
    {
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_PING_REQ, GamePacketHandleFuncList::HandleGamePingReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_HAND_SHAKING_REQ, GamePacketHandleFuncList::HandleGameHandShakingReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_CREATE_ROOM_REQ, GamePacketHandleFuncList::HandleGameCreateRoomReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_ROOM_LIST_REQ, GamePacketHandleFuncList::HandleGameRoomListReq);
    }

    void* GamePacketHandleFuncList::getPacketData(const Packet& packet)
    {
        return packet.GetBody() + sizeof(PacketHandler::cmdType);
    }

    bool GamePacketHandleFuncList::HandleGamePingReq(const Session* session, const Packet& packet)
    {
        LOG_DEBUG(L"on ping packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameHandShakingReq(const Session* session, const Packet& packet)
    {
        void* packetData = getPacketData(packet);
        PARSER_PROTO_PACKET_DATA(GameHandShakingReq, req, packetData);

        const auto clientPacketVersion = req.packetversion();
        const auto serverPacketVersion = static_cast<int>(GamePacketInfo::GAME_PACKET_INFO_VERSION);
        if (serverPacketVersion != clientPacketVersion)
        {
            LOG_ERROR(L"not equal packet version, clientPacketVersion:{}, serverPacketVersion:{}", clientPacketVersion, serverPacketVersion);
            return false;
        }


        LOG_DEBUG(L"on hand shaking packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameCreateRoomReq(const Session* session, const Packet& packet)
    {
        void* packetData = getPacketData(packet);
        PARSER_PROTO_PACKET_DATA(GameCreateRoomReq, req, packetData);

        LOG_DEBUG(L"on create room packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameRoomListReq(const Session* session, const Packet& packet)
    {
        void* packetData = getPacketData(packet);
        PARSER_PROTO_PACKET_DATA(GameRoomListReq, req, packetData);

        LOG_DEBUG(L"on room list packet, sessionId:{}", session->GetId());
        return true;
    }
}

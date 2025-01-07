#include "GamePacketHandleFuncList.h"
#include "PacketHandler.h"
#include "StringConverter.h"
#include "PacketBuffer.h"
#include "PacketBufferPool.h"
#include "../Packet/GamePacket/Cpp/GamaPacket.pb.h"
#include <codecvt>
#include "StopWatch.h"

#define REGIST_HANDLE(cmd, handleFun) _packetHandler->RegistHandler(cmd, std::bind(&handleFun, this, std::placeholders::_1, std::placeholders::_2));

#define PARSER_PROTO_PACKET_DATA(protoPacket, req, packet) \
void* packetData = getPacketData(packet); \
protoPacket req; \
const int protoBufferSize = packet.GetBodySize() - sizeof(PacketHandler::cmdType); \
if (!req.ParseFromArray(packetData, protoBufferSize)){ \
auto reqName = typeid(req).name(); \
LOG_FETAL(L"fail req parser, sessionId:{}, req-typeid:{}, protoBufferSize:{}", session->GetId(), StringConverter::StrA2WUseUTF8(reqName)->c_str(), protoBufferSize ); return false; }

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
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_CHAT_REQ, GamePacketHandleFuncList::HandleGameChatReq);
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
        ;
        PARSER_PROTO_PACKET_DATA(GameHandShakingReq, req, packet);

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
        PARSER_PROTO_PACKET_DATA(GameCreateRoomReq, req, packet);

        LOG_DEBUG(L"on create room packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameRoomListReq(const Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameRoomListReq, req, packet);

        LOG_DEBUG(L"on room list packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameChatReq(const Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameChatReq, req, packet);
        LOG_DEBUG(L"on chat req packet, sessionId:{}, name:{}, msg:{}", session->GetId(), StringConverter::StrA2WUseUTF8(req.name())->c_str(), StringConverter::StrA2WUseUTF8(req.msg())->c_str());

        GameChatOk response;
        response.set_name(req.name());
        response.set_msg(req.msg());
        ;
        Packet sendPacket;
        std::shared_ptr<PacketBuffer> packetBuffer{ PACKET_BUFFER_POOL().Acquire(), [](PacketBuffer* obj) { PACKET_BUFFER_POOL().Release(obj); } };
        sendPacket.SetPacketBuffer(packetBuffer);


        PacketHandler::cmdType cmd = GAME_PACKET_CMD_CHAT_OK;
        char responseBody[256] = { 0, };
        const auto responseSize = response.ByteSizeLong();
        response.SerializeToArray(responseBody, (int)responseSize);

        sendPacket.Add(&cmd, sizeof(PacketHandler::cmdType));
        sendPacket.Add(responseBody, (int)responseSize);

        const_cast<Session*>(session)->Send(sendPacket);
        return true;
    }
}

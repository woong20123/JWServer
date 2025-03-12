#include "GamePacketHandleFuncList.h"
#include "PacketHandler.h"
#include "StringConverter.h"
#include "PacketBuffer.h"
#include "PacketBufferPool.h"
#include "PacketHelper.hpp"
#include "User.h"
#include "Network.h"
#include "SampleServer.h"
#include "../Packet/GamePacket/Cpp/GamaPacket.pb.h"
#include <codecvt>
#include "StopWatch.h"
#include "WorldSerializeObject.h"
#include "SerializerManager.h"

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
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_LOGIN_REQ, GamePacketHandleFuncList::HandleGameLoginReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_CREATE_ROOM_REQ, GamePacketHandleFuncList::HandleGameCreateRoomReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_ROOM_LIST_REQ, GamePacketHandleFuncList::HandleGameRoomListReq);
        REGIST_HANDLE(GamePacketCmd::GAME_PACKET_CMD_CHAT_REQ, GamePacketHandleFuncList::HandleGameChatReq);
    }

    void* GamePacketHandleFuncList::getPacketData(const Packet& packet)
    {
        return packet.GetBody() + sizeof(PacketHandler::cmdType);
    }

    bool GamePacketHandleFuncList::HandleGamePingReq(Session* session, const Packet& packet)
    {
        LOG_DEBUG(L"on ping packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameLoginReq(Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameLoginReq, req, packet);

        const auto& name = req.name();
        const auto& clientAuthKey = req.authkey();
        const auto& serverAuthKey = static_cast<int>(GamePacketInfo::GAME_PACKET_AUTH_KEY);
        if (serverAuthKey != clientAuthKey)
        {
            LOG_ERROR(L"not equal auth key, sessionId:{}, clientAuthKey:{}, serverAuthKey:{}", session->GetId(), clientAuthKey, serverAuthKey);
            GameLoginFail gameLoginFail;
            gameLoginFail.set_errcode(ERROR_CODE_LOGIN_FAIL_INVALID_AUTH);
            PacketHelper::Send(session, GAME_PACKET_CMD_LOGIN_FAIL, gameLoginFail);
            return true;
        }

        const auto clientPacketVersion = req.packetversion();
        const auto serverPacketVersion = static_cast<int>(GamePacketInfo::GAME_PACKET_INFO_VERSION);
        if (serverPacketVersion != clientPacketVersion)
        {
            LOG_ERROR(L"not equal packet version, sessionId:{}, clientPacketVersion:{}, serverPacketVersion:{}", session->GetId(), clientPacketVersion, serverPacketVersion);
            GameLoginFail gameLoginFail;
            gameLoginFail.set_errcode(ERROR_CODE_LOGIN_FAIL_INVALID_NAME);
            PacketHelper::Send(session, GAME_PACKET_CMD_LOGIN_FAIL, gameLoginFail);
            return true;
        }

        std::shared_ptr<User> user = std::make_shared<User>();
        user->Initialize(NETWORK().GetSession(session->GetPortId(), session->GetIndex()), name);
        if (const auto registerUserResult = SAMPLE_SERVER().GetWorld()->RegistUser(user);
            registerUserResult != REGITER_USER_RESULT_SUCCESS)
        {
            const auto& userName = StringConverter::StrA2WUseUTF8(user->GetName().data()).value_or(L"empty");
            LOG_ERROR(L"register User Fail, sessionId:{}, UserName:{}, result:{}", session->GetId(), userName.c_str(), (int)registerUserResult);
            GameLoginFail gameLoginFail;
            gameLoginFail.set_errcode(ERROR_CODE_LOGIN_FAIL_DUPLICATE_NAME);
            PacketHelper::Send(user.get(), GAME_PACKET_CMD_LOGIN_FAIL, gameLoginFail);
            return true;
        }

        session->SetChannelKey(user->GetUserKey());

        GameLoginOk gameLoginOk;
        gameLoginOk.set_name(name);
        gameLoginOk.set_userid(user->GetUserKey());
        PacketHelper::Send(user.get(), GAME_PACKET_CMD_LOGIN_OK, gameLoginOk);

        LOG_DEBUG(L"on login packet, sessionId:{}, userKey:{}", session->GetId(), user->GetUserKey());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameCreateRoomReq(Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameCreateRoomReq, req, packet);

        std::shared_ptr<CreateRoomTask> createRoomTask = std::make_shared<CreateRoomTask>();

        const auto user = SAMPLE_SERVER().GetWorld()->FindUser(session->GetChannelKey());

        createRoomTask->Initialize(user, req.name());
        auto worldSerializerKey = SAMPLE_SERVER().GetWorld()->GetSerializerKey();
        if (!SERIALIZER_MANAGER().Post(worldSerializerKey, createRoomTask))
        {
            LOG_ERROR(L"Fail CreateRoomTask Post");
        }

        LOG_DEBUG(L"on create room packet, sessionId:{}", session->GetId());
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameRoomListReq(Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameRoomListReq, req, packet);

        std::shared_ptr<GameRoomListTask> gameRoomListTask = std::make_shared<GameRoomListTask>();

        auto worldSerializerKey = SAMPLE_SERVER().GetWorld()->GetSerializerKey();
        if (!SERIALIZER_MANAGER().Post(worldSerializerKey, gameRoomListTask))
        {
            LOG_ERROR(L"Fail WorldChatTask Post");
        }
        return true;
    }

    bool GamePacketHandleFuncList::HandleGameChatReq(Session* session, const Packet& packet)
    {
        PARSER_PROTO_PACKET_DATA(GameChatReq, req, packet);
        LOG_DEBUG(L"on chat req packet, sessionId:{}, name:{}, msg:{}", session->GetId(), StringConverter::StrA2WUseUTF8(req.name())->c_str(), StringConverter::StrA2WUseUTF8(req.msg())->c_str());

        std::shared_ptr<WorldChatTask> worldChatTask = std::make_shared<WorldChatTask>();
        worldChatTask->Initialize(req.name(), req.msg());
        auto worldSerializerKey = SAMPLE_SERVER().GetWorld()->GetSerializerKey();
        if (!SERIALIZER_MANAGER().Post(worldSerializerKey, worldChatTask))
        {
            LOG_ERROR(L"Fail WorldChatTask Post");
        }
        return true;
    }

}

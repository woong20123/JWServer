#include "WorldSerializeObject.h"
#include "SerializerInfo.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketHelper.hpp"
#include "SampleServer.h"
#include "RoomManager.h"
#include "User.h"
#include "../Packet/GamePacket/Cpp/GamaPacket.pb.h"

namespace jw
{
    WorldSerializeObject::WorldSerializeObject(int32_t id) : SerializeObject(EXPAND_SERAILIZE_OBJECT_TYPE_WORLD, id)
    {

    }

    WorldChatTask::WorldChatTask() : WorldSerializeObject(WORLD_SERIALIZE_OBJECT_ID_CHAT)
    {}

    WorldChatTask::~WorldChatTask()
    {
        LOG_DEBUG(L"WorldChatTask::~WorldChatTask() called");
    }

    void WorldChatTask::Initialize(const std::string& name, const std::string& msg)
    {
        this->_name = name;
        this->_message = msg;
    }

    void WorldChatTask::Execute()
    {
        GameChatOk gameChatOk;
        gameChatOk.set_name(_name);
        gameChatOk.set_msg(_message);

        Packet sendPacket;
        sendPacket.Allocate();

        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_CHAT_OK, gameChatOk);
        SAMPLE_SERVER().GetWorld()->BroadcastPacket(sendPacket);

        LOG_DEBUG(L"WorldChatTask::Execute() called");
    }

    CreateRoomTask::CreateRoomTask() : WorldSerializeObject(WORLD_SERIALIZE_OBJECT_ID_CREATE_ROOM)
    {
    }

    CreateRoomTask::~CreateRoomTask()
    {
        LOG_DEBUG(L"CreateRoomTask::~CreateRoomTask() called");
    }

    void CreateRoomTask::Initialize(const std::shared_ptr<User>& user, const std::string& roomName)
    {
        _user = user;
        _roomName = roomName;
    }

    void CreateRoomTask::Execute()
    {
        if (!_user) return;

        if (_user->IsEnterRoom())
        {
            LOG_ERROR(L"Already EnterRoom, userId:{}", _user->GetUserKey());
            sendFail(ERROR_CODE_ALREADY_ENTERED_ROOM);
            return;
        }

        const auto userKey = _user->GetUserKey();
        const auto userName = _user->GetName();

        const auto result = SAMPLE_SERVER().GetRoomManager()->CreateRoom(_roomName, { userName.data(), userKey });

        if (!result._isSuccess)
        {
            LOG_ERROR(L"CreateRoomTask::Execute() failed");
            sendFail(ERROR_CODE_CREATE_ROOM_FAIL);
            return;
        }

        _user->SetEnterRoomId(result._roomId);

        RoomListInfo* roomListInfo = new RoomListInfo();
        roomListInfo->set_roomid(result._roomId);
        roomListInfo->set_name(_roomName);
        roomListInfo->set_hostuserid(userKey);
        roomListInfo->set_hostusername(userName.data());

        GameCreateRoomOk gameCreateRoomOk;
        gameCreateRoomOk.set_allocated_roominfo(roomListInfo);

        Packet sendPacket;
        sendPacket.Allocate();

        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_CREATE_ROOM_OK, gameCreateRoomOk);
        _user->Send(sendPacket);
    }

    void CreateRoomTask::sendFail(int32_t errcode)
    {
        GameCreateRoomFail gameCreateRoomFail;
        gameCreateRoomFail.set_errcode(static_cast<ErrorCode>(errcode));

        Packet sendPacket;
        sendPacket.Allocate();

        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_CREATE_ROOM_FAIL, gameCreateRoomFail);
        _user->Send(sendPacket);
    }

    GameRoomListTask::GameRoomListTask() : WorldSerializeObject(WORLD_SERIALIZE_OBJECT_ID_ROOM_LIST)
    {
    }

    GameRoomListTask::~GameRoomListTask()
    {
        LOG_DEBUG(L"CreateRoomTask::~CreateRoomTask() called");
    }

    void GameRoomListTask::Initialize(const std::shared_ptr<User>& user)
    {
        _user = user;
    }

    void GameRoomListTask::Execute()
    {
        GameRoomListOk gameRoomListOk;
        auto roomInfoList = SAMPLE_SERVER().GetRoomManager()->GetRoomList();
        for (const auto& roomInfo : roomInfoList)
        {
            RoomListInfo* roomListInfo = gameRoomListOk.add_roominfo();
            roomListInfo->set_roomid(roomInfo._roomId);
            roomListInfo->set_name(roomInfo._roomName);
            roomListInfo->set_hostuserid(roomInfo._hostUserId);
            roomListInfo->set_hostusername(roomInfo._hostUserName);
        }
        Packet sendPacket;
        sendPacket.Allocate();
        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_LIST_OK, gameRoomListOk);
        _user->Send(sendPacket);
    }

    CreateRoomNotifyTask::CreateRoomNotifyTask() : WorldSerializeObject(WORLD_SERIALIZE_OBJECT_ID_CREATE_ROOM_NOTIFY)
    {
    }

    CreateRoomNotifyTask::~CreateRoomNotifyTask()
    {
        LOG_DEBUG(L"CreateRoomNotifyTask::~CreateRoomNotifyTask() called");
    }

    void CreateRoomNotifyTask::Initialize(int64_t roomId, std::string& roomName, int64_t hostUserId, std::string& hostName)
    {
        _roomId = roomId;
        _roomName = roomName;
        _hostUserId = hostUserId;
        _hostUserName = hostName;
    }

    void CreateRoomNotifyTask::Execute()
    {
        RoomListInfo* roomListInfo = new RoomListInfo();
        roomListInfo->set_roomid(_roomId);
        roomListInfo->set_name(_roomName);
        roomListInfo->set_hostuserid(_hostUserId);
        roomListInfo->set_hostusername(_hostUserName);

        GameCreateRoomNotify gameCreateRoomNotify;
        gameCreateRoomNotify.set_allocated_roominfo(roomListInfo);


        Packet sendPacket;
        sendPacket.Allocate();

        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_CREATE_ROOM_NOTIFY, gameCreateRoomNotify);
        SAMPLE_SERVER().GetWorld()->BroadcastPacket(sendPacket);

        LOG_DEBUG(L"CreateRoomNotifyTask::Execute() called, roomId:{}", _roomId);
    }

    DestoroyRoomNotifyTask::DestoroyRoomNotifyTask() : WorldSerializeObject(WORLD_SERIALIZE_OBJECT_ID_DESTOROY_ROOM_NOTIFY)
    {
    }

    DestoroyRoomNotifyTask::~DestoroyRoomNotifyTask()
    {
        LOG_DEBUG(L"DestoroyRoomTask::~DestoroyRoomTask() called");
    }

    void DestoroyRoomNotifyTask::Initialize(int64_t roomId)
    {
        _roomId = roomId;
    }

    void DestoroyRoomNotifyTask::Execute()
    {
        GameDestroyRoomNotify gameDestroyRoomNotify;
        gameDestroyRoomNotify.set_roomid(_roomId);

        Packet sendPacket;
        sendPacket.Allocate();

        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_DESTROY_ROOM_NOTIFY, gameDestroyRoomNotify);
        SAMPLE_SERVER().GetWorld()->BroadcastPacket(sendPacket);

        LOG_DEBUG(L"DestoroyRoomNotifyTask::Execute() called, roomId:{}", _roomId);
    }

}
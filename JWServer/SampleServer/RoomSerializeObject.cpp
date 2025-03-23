#include "RoomSerializeObject.h"
#include "SerializerInfo.h"
#include "SampleServer.h"
#include "Room.h"
#include "RoomManager.h"
#include "User.h"
#include "Packet.h"
#include "PacketHelper.hpp"
#include "../Packet/GamePacket/Cpp/GamaPacket.pb.h"

namespace jw
{
    RoomSerializeObject::RoomSerializeObject(int32_t id) : SerializeObject(EXPAND_SERAILIZE_OBJECT_TYPE_ROOM, id)
    {
    }

    RoomChatTask::RoomChatTask() : RoomSerializeObject(ROOM_SERIALIZE_OBJECT_ID_ROOM_CHAT)
    {

    }

    RoomChatTask::~RoomChatTask()
    {

    }

    void RoomChatTask::Initialize(const int64_t roomId, const std::shared_ptr<User>& user, const std::string& msg)
    {
        _roomId = roomId;
        _user = user;
        _message = msg;
    }

    void RoomChatTask::Execute()
    {
        const auto memberIdList = SAMPLE_SERVER().GetRoomManager()->GetRoomMemberIds(_roomId);
        if (memberIdList.empty())
        {
            LOG_ERROR(L"empty memberIdList, roomId:{}", _roomId);
            return;
        }

        GameRoomChatOk roomChatOk;
        roomChatOk.set_roomid(_roomId);
        roomChatOk.set_name(_user->GetName().data());
        roomChatOk.set_msg(_message);

        Packet sendPacket;
        sendPacket.Allocate();
        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_CHAT_OK, roomChatOk);
        for (const auto& memberId : memberIdList)
        {
            const auto& memberUser = SAMPLE_SERVER().GetWorld()->FindUser(memberId);
            if (memberUser) memberUser->Send(sendPacket);
        }

    }

    RoomEnterTask::RoomEnterTask() : RoomSerializeObject(ROOM_SERIALIZE_OBJECT_ID_ROOM_CHAT)
    {}

    RoomEnterTask::~RoomEnterTask()
    { }

    void RoomEnterTask::Initialize(const int64_t roomId, const std::shared_ptr<User>& user)
    {
        _roomId = roomId;
        _user = user;
    }

    void RoomEnterTask::Execute()
    {
        const auto result = SAMPLE_SERVER().GetRoomManager()->EnterRoom(_roomId, _user->GetUserKey(), _user->GetName().data());
        if (ROOM_RESULT_SUCCESS != result)
        {
            switch (result)
            {
            case ROOM_RESULT_NOT_FIND_ROOM:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ENTER_ROOM_NOT_FIND_ROOM);
                break;
            case ROOM_RESULT_EXIST_USER:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ENTER_ROOM_EXIST_USER);
                return;
            default:
                sendFail(ERROR_CODE_UNKNOWN_FAIL);
                return;
            }
        }
        _user->SetEnterRoomId(_roomId);

        // RoomEnterOk 패킷 전송
        sendOk();
        // 방안에 인원에게 Notify 패킷 전송
        sendNotifyEnterUserInfo();
    }

    void RoomEnterTask::sendNotifyEnterUserInfo()
    {
        const auto memberIdList = SAMPLE_SERVER().GetRoomManager()->GetRoomMemberIds(_roomId);
        if (memberIdList.empty())
        {
            return;
        }

        GameRoomEnterNotify roomEnterNotify;
        UserInfo* userInfo = new UserInfo();
        userInfo->set_userid(_user->GetUserKey());
        userInfo->set_username(_user->GetName().data());
        roomEnterNotify.set_allocated_enteruserinfo(userInfo);
        roomEnterNotify.set_roomid(_roomId);

        Packet notifyPacket;
        notifyPacket.Allocate();
        PacketHelper::ComposeProtoPacket(notifyPacket, GAME_PACKET_CMD_ROOM_ENTER_NOTIFY, roomEnterNotify);

        for (const auto& memberId : memberIdList)
        {
            if (_user->GetUserKey() == memberId) continue;
            const auto& memberUser = SAMPLE_SERVER().GetWorld()->FindUser(memberId);
            if (memberUser) memberUser->Send(notifyPacket);
        }
    }

    void RoomEnterTask::sendOk()
    {
        const auto& memberIdList = SAMPLE_SERVER().GetRoomManager()->GetRoomMemberInfoList(_roomId);
        const auto& roomInfo = SAMPLE_SERVER().GetRoomManager()->GetRoomInfo(_roomId);
        GameRoomEnterOk roomEnterOk;
        for (const auto& memberInfo : memberIdList)
        {
            UserInfo* userInfo = roomEnterOk.add_memberuserinfos();
            userInfo->set_userid(memberInfo._userId);
            userInfo->set_username(memberInfo._name);
        }
        RoomListInfo* roomListInfo = new RoomListInfo();
        roomListInfo->set_roomid(roomInfo._roomId);
        roomListInfo->set_name(roomInfo._roomName);
        roomListInfo->set_hostuserid(roomInfo._hostUserId);
        roomListInfo->set_hostusername(roomInfo._hostUserName);
        roomEnterOk.set_allocated_roominfo(roomListInfo);

        Packet sendPacket;
        sendPacket.Allocate();
        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_ENTER_OK, roomEnterOk);

        _user->Send(sendPacket);
    }

    void RoomEnterTask::sendFail(int32_t errcode)
    {
        GameRoomEnterFail gameRoomEnterFail;
        gameRoomEnterFail.set_errcode(static_cast<ErrorCode>(errcode));

        Packet sendPacket;
        sendPacket.Allocate();
        PacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_ENTER_FAIL, gameRoomEnterFail);

        _user->Send(sendPacket);
    }
}
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

    // ignoreUserKey가 없을 경우 0으로 설정
    class RoomSerializeObjectCommon
    {
    public:
        static constexpr int64_t NONE_IGNORE_USER_KEY = 0;
        static void SendPacketBroadCastInRoomUser(const int64_t roomId, Packet& sendPackets, const int64_t ignoreUserKey)
        {
            const auto memberIdList = SAMPLE_SERVER().GetRoomManager()->GetRoomMemberIds(roomId);
            for (auto& memberId : memberIdList)
            {
                if (ignoreUserKey == memberId) continue;
                const auto& memberUser = SAMPLE_SERVER().GetWorld()->FindUser(memberId);
                if (memberUser) memberUser->Send(sendPackets);
            }
        }

        template<typename PacketType> requires std::derived_from<PacketType, ::google::protobuf::Message>
        static void SendFail(const int errCode, const int packetCommand, const std::shared_ptr<User>& user)
        {
            PacketType failPacketType;
            failPacketType.set_errcode(static_cast<ErrorCode>(errCode));

            Packet sendPacket;
            sendPacket.Allocate();
            ProtoBufPacketHelper::ComposeProtoPacket(sendPacket, packetCommand, failPacketType);

            user->Send(sendPacket);
        }
    };

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
        if (!SAMPLE_SERVER().GetRoomManager()->HasMembers(_roomId))
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
        ProtoBufPacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_CHAT_OK, roomChatOk);

        RoomSerializeObjectCommon::SendPacketBroadCastInRoomUser(_roomId, sendPacket, RoomSerializeObjectCommon::NONE_IGNORE_USER_KEY);
    }

    RoomEnterTask::RoomEnterTask() : RoomSerializeObject(ROOM_SERIALIZE_OBJECT_ID_ROOM_CHAT)
    {}

    RoomEnterTask::~RoomEnterTask()
    {}

    void RoomEnterTask::Initialize(const int64_t roomId, const std::shared_ptr<User>& user)
    {
        _roomId = roomId;
        _user = user;
    }

    void RoomEnterTask::Execute()
    {
        if (_user->IsEnterRoom())
        {
            LOG_ERROR(L"Already EnterRoom, roomId:{}", _roomId);
            sendFail(ERROR_CODE_ALREADY_ENTERED_ROOM);
            return;
        }

        const auto result = SAMPLE_SERVER().GetRoomManager()->EnterRoom(_roomId, _user->GetUserKey(), _user->GetName().data());
        if (ROOM_RESULT_SUCCESS != result)
        {
            switch (result)
            {
            case ROOM_RESULT_NOT_FIND_ROOM:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ROOM_NOT_FIND_ROOM);
                return;
            case ROOM_RESULT_EXIST_USER:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ROOM_EXIST_USER);
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
        if (!SAMPLE_SERVER().GetRoomManager()->HasMembers(_roomId))
        {
            LOG_ERROR(L"empty memberIdList, roomId:{}", _roomId);
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
        ProtoBufPacketHelper::ComposeProtoPacket(notifyPacket, GAME_PACKET_CMD_ROOM_ENTER_NOTIFY, roomEnterNotify);

        RoomSerializeObjectCommon::SendPacketBroadCastInRoomUser(_roomId, notifyPacket, _user->GetUserKey());
    }

    void RoomEnterTask::sendOk()
    {
        const auto& memberIdList = SAMPLE_SERVER().GetRoomManager()->GetRoomMemberInfoList(_roomId);
        const auto& roomInfo = SAMPLE_SERVER().GetRoomManager()->GetRoomInfo(_roomId);
        GameRoomEnterOk roomEnterOk;
        for (auto& memberInfo : memberIdList)
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
        ProtoBufPacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_ENTER_OK, roomEnterOk);

        _user->Send(sendPacket);
    }

    void RoomEnterTask::sendFail(int32_t errcode)
    {
        RoomSerializeObjectCommon::SendFail<GameRoomEnterFail>(errcode, GAME_PACKET_CMD_ROOM_ENTER_FAIL, _user);
    }

    RoomLeaveTask::RoomLeaveTask() : RoomSerializeObject(ROOM_SERIALIZE_OBJECT_ID_ROOM_LEAVE)
    {}

    RoomLeaveTask::~RoomLeaveTask()
    {}

    void RoomLeaveTask::Initialize(const int64_t roomId, const std::shared_ptr<User>& user)
    {
        _roomId = roomId;
        _user = user;
    }

    void RoomLeaveTask::Execute()
    {
        const auto result = SAMPLE_SERVER().GetRoomManager()->LeaveRoom(_roomId, _user->GetUserKey());
        if (ROOM_RESULT_SUCCESS != result)
        {
            switch (result)
            {
            case ROOM_RESULT_NOT_FIND_ROOM:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ROOM_NOT_FIND_ROOM);
                break;
            case ROOM_RESULT_EXIST_USER:
                LOG_ERROR(L"Fail FindRoom, roomId:{}", _roomId);
                sendFail(ERROR_CODE_ROOM_EXIST_USER);
                return;
            default:
                sendFail(ERROR_CODE_UNKNOWN_FAIL);
                return;
            }
        }
        _user->SetEnterRoomId(INVALID_ROOM_ID);

        // RoomEnterOk 패킷 전송
        sendOk();
        // 방안에 인원에게 Notify 패킷 전송
        sendNotifyLeaveUserInfo();
    }

    void RoomLeaveTask::sendOk()
    {
        GameRoomLeaveOk roomLeaveOk;
        roomLeaveOk.set_roomid(_roomId);

        Packet sendPacket;
        sendPacket.Allocate();
        ProtoBufPacketHelper::ComposeProtoPacket(sendPacket, GAME_PACKET_CMD_ROOM_LEAVE_OK, roomLeaveOk);

        _user->Send(sendPacket);
    }

    void RoomLeaveTask::sendNotifyLeaveUserInfo()
    {
        if (!SAMPLE_SERVER().GetRoomManager()->HasMembers(_roomId))
        {
            LOG_ERROR(L"empty memberIdList, roomId:{}", _roomId);
            return;
        }

        GameRoomLeaveNotify roomLeaveNotify;
        UserInfo* userInfo = new UserInfo();
        userInfo->set_userid(_user->GetUserKey());
        userInfo->set_username(_user->GetName().data());
        roomLeaveNotify.set_allocated_leaveuserinfo(userInfo);
        roomLeaveNotify.set_roomid(_roomId);

        Packet notifyPacket;
        notifyPacket.Allocate();
        ProtoBufPacketHelper::ComposeProtoPacket(notifyPacket, GAME_PACKET_CMD_ROOM_LEAVE_NOTIFY, roomLeaveNotify);

        RoomSerializeObjectCommon::SendPacketBroadCastInRoomUser(_roomId, notifyPacket, _user->GetUserKey());
    }

    void RoomLeaveTask::sendFail(int32_t errcode)
    {
        RoomSerializeObjectCommon::SendFail<GameRoomLeaveFail>(errcode, GAME_PACKET_CMD_ROOM_LEAVE_FAIL, _user);
    }

}
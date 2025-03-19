#include "RoomSerializeObject.h"
#include "SerializerInfo.h"
#include "SampleServer.h"
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

    RoomChatTask::RoomChatTask() : RoomSerializeObject(SERIALIZE_OBJECT_ROOM_ID_CHAT)
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
}
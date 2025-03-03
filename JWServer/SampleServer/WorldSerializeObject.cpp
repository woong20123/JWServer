#include "WorldSerializeObject.h"
#include "SerializerInfo.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketHelper.hpp"
#include "SampleServer.h"
#include "../Packet/GamePacket/Cpp/GamaPacket.pb.h"

namespace jw
{
    WorldSerializeObject::WorldSerializeObject(int32_t id) : SerializeObject(EXPAND_SERAILIZE_OBJECT_TYPE_WORLD, id)
    {

    }

    WorldChatTask::WorldChatTask() : WorldSerializeObject(SERIALIZE_OBJECT_WORLD_ID_CHAT)
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
}
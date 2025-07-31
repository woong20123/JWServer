#pragma once
#ifndef __JW_PACKET_HELPER_HPP__
#define __JW_PACKET_HELPER_HPP__

#include "Packet.h"
#include "Session.h"
#include "Channel.h"
#include "PacketBuffer.h"
#include "PacketHandler.h"

namespace jw
{
    // concept 적용시 Send의 오버로드 중의성으로 에러가 발생. 일단은 cencept에 대한 정의만 구성합니다. 
    template<typename T>
    concept protoBufMsg = std::derived_from<T, ::google::protobuf::Message>;

    class Channel;
    class ProtoBufPacketHelper
    {
    public:
        using commandType = PacketHandler::cmdType;

        template<typename T>
        static void ComposeProtoPacket(Packet& packet, const commandType cmd, T& packetData)
        {
            char packetDataBytes[PacketBuffer::BUFFER_SIZE] = { 0, };
            const auto responseSize = packetData.ByteSizeLong();
            packetData.SerializeToArray(packetDataBytes, (int)responseSize);

            packet.Add(&cmd, sizeof(commandType));
            packet.Add(packetDataBytes, (int)responseSize);
        }

        template<typename T>
        static void Send(Session* session, const commandType cmd, T& packetData)
        {
            Packet sendPacket;
            sendPacket.Allocate();
            ComposeProtoPacket(sendPacket, cmd, packetData);
            session->Send(sendPacket);
        }

        template<typename T>
        static void Send(Channel* channel, const commandType cmd, T& packetData)
        {
            Packet sendPacket;
            sendPacket.Allocate();
            ComposeProtoPacket(sendPacket, cmd, packetData);
            channel->Send(sendPacket);
        }
    };
}

#endif // !__JW_PACKET_HELPER_HPP__
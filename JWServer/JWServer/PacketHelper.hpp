#pragma once
#ifndef __JW_PACKET_HELPER_HPP__
#define __JW_PACKET_HELPER_HPP__

#include "Packet.h"
#include "Session.h"
#include "Channel.h"
#include "PacketHandler.h"

namespace jw
{
    class Channel;
    class PacketHelper
    {
    public:
        template<typename T>
        static void ComposeProtoPacket(Packet& packet, const PacketHandler::cmdType cmd, T& packetData)
        {
            char packetDataBytes[Packet::MAX_SIZE] = { 0, };
            const auto responseSize = packetData.ByteSizeLong();
            packetData.SerializeToArray(packetDataBytes, (int)responseSize);

            packet.Add(&cmd, sizeof(PacketHandler::cmdType));
            packet.Add(packetDataBytes, (int)responseSize);
        }

        template<typename T>
        static void Send(Session* session, const PacketHandler::cmdType cmd, T& packetData)
        {
            Packet sendPacket;
            sendPacket.Allocate();
            ComposeProtoPacket(sendPacket, cmd, packetData);
            session->Send(sendPacket);
        }

        template<typename T>
        static void Send(Channel* channel, const PacketHandler::cmdType cmd, T& packetData)
        {
            Packet sendPacket;
            sendPacket.Allocate();
            ComposeProtoPacket(sendPacket, cmd, packetData);
            channel->Send(sendPacket);
        }
    };
}

#endif // !__JW_PACKET_HELPER_HPP__
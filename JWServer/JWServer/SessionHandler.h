﻿#pragma once
#ifndef __JW_SESSION_HANDLER_H__
#define __JW_SESSION_HANDLER_H__
#include <memory>

namespace jw
{
    class Session;
    class Packet;
    class PacketHandler;
    class SessionHandler
    {
    public:
        virtual void Initialize(std::shared_ptr<PacketHandler> packetHandler) = 0;

        // 세션 풀을 사용하는 경우 풀에서 세션을 할당 받을 때 호출 됩니다. 
        virtual void OnCreated(const Session* session) const = 0;
        // 세션 풀을 사용하는 경우 풀에 세션을 반납할 때 호출 합니다.
        virtual void OnDestroyed(const Session* session) const = 0;
        virtual bool OnAccepted(const Session* session) const = 0;
        virtual bool OnConnected(const Session* session) const = 0;
        virtual void OnClosed(const Session* session) const = 0;
        virtual void OnPacket(const Session* session, const Packet& packet) const = 0;
    };
}
#endif // !__JW_SESSION_HANDLER_H__


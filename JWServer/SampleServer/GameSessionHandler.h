#pragma once
#ifndef __JW_GAME_SESSION_HANDLER_H__
#define __JW_GAME_SESSION_HANDLER_H__
#include "SessionHandler.h"
namespace jw
{
    class GameSessionHandler : public SessionHandler
    {
    public:
        void Initialize(std::shared_ptr<PacketHandler> packetHandler) override;
        void OnCreated(const Session* session) const override;
        void OnDestroyed(const Session* session) const override;
        bool OnAccepted(const Session* session) const override;
        bool OnConnected(const Session* session) const override;
        void OnClosed(const Session* session) const override;
        void OnPacket(const Session* session, const Packet& packet) const override;
    private:
        std::shared_ptr<PacketHandler> _packetHandler;
    };
}
#endif // !__JW_GAME_SESSION_HANDLER_H__ 


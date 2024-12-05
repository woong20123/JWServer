#include "GameSessionHandler.h"
#include "Session.h"
#include "SessionBuffer.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketHandler.h"

namespace jw
{
    void GameSessionHandler::Initialize(std::shared_ptr<PacketHandler> packetHandler)
    {
        _packetHandler = packetHandler;
    }

    void GameSessionHandler::OnCreated(const Session* session) const
    {
        LOG_INFO(L"On Create Session, id:{}", session->GetId());
    }
    void GameSessionHandler::OnDestroyed(const Session* session) const
    {
        LOG_INFO(L"On Destroyed Session, id:{}", session->GetId());
    }

    bool GameSessionHandler::OnAccepted(const Session* session) const
    {
        LOG_INFO(L"On Accepted Session, id:{}", session->GetId());
        return true;
    }

    bool GameSessionHandler::OnConnected(const Session* session) const
    {
        LOG_INFO(L"On Connected Session, id:{}", session->GetId());
        return false;
    }
    void GameSessionHandler::OnClosed(const Session* session) const
    {
        LOG_INFO(L"On Closed Session, id:{}", session->GetId());
    }

    void GameSessionHandler::OnPacket(const Session* session, const Packet& packet) const
    {
        LOG_INFO(L"On Packet Session, id:{}", session->GetId());
        _packetHandler->HandlePacket(session, packet);
    }
}

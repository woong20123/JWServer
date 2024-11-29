#include "GameSessionHandler.h"
#include "Session.h"
#include "SessionBuffer.h"
#include "Logger.h"

namespace jw
{
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
}

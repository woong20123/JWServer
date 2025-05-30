﻿#include "Channel.h"
#include "Packet.h"
#include "Session.h"

namespace jw
{
    Channel::Channel() : Channel{ nullptr }
    {
    }

    Channel::Channel(const std::shared_ptr<Session>& session) : _session(session)
    {
    }

    void Channel::SetSession(const std::shared_ptr<Session>& session)
    {
        _session = session;
    }

    void Channel::RemovSession()
    {
        _session.reset();
    }

    bool Channel::Send(Packet& packet)
    {
        _session->Send(packet);
        return true;
    }
}
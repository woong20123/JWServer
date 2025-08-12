#pragma once
#ifndef __JW_CHANNEL_H__
#define __JW_CHANNEL_H__
#include <memory>

namespace jw
{
    class Session;
    class Packet;
    class Channel
    {
    public:
        Channel();
        Channel(const std::shared_ptr<Session>& session);
        virtual ~Channel() = default;

        void SetSession(const std::shared_ptr<Session>& session);
        void RemovSession();

        virtual bool Send(Packet& packet);
        bool Close(uint32_t reason);
    protected:
        std::shared_ptr<Session> _session;
    };
}

#endif // !__JW_CHANNEL_H__


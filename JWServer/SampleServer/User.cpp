#include "User.h"
#include "Session.h"
#include "Logger.h"
#include "SampleServer.h"
#include "RoomManager.h"
#include "World.h"

namespace jw
{
    User::User() : _userKey{ INVALID_USER_KEY }, _enterRoomId{ INVALID_ROOM_ID }
    {}
    User::User(std::shared_ptr<Session>& session) : Channel(session), _userKey{ INVALID_USER_KEY }, _enterRoomId{ INVALID_ROOM_ID }
    {}

    void User::Initialize(const std::shared_ptr<Session>& session, const std::string& name)
    {
        setSession(session);
        setName(name);
    }

    std::string_view User::GetName() const
    {
        return _name;
    }

    void User::SetUserKey(const int64_t userkey)
    {
        _userKey = userkey;
    }

    int64_t User::GetUserKey() const
    {
        return _userKey;
    }

    void User::SetEnterRoomId(const int64_t roomId)
    {
        _enterRoomId = roomId;
    }

    int64_t User::GetEnterRoomId() const
    {
        return _enterRoomId;
    }

    bool User::IsEnterRoom() const
    {
        return _enterRoomId != INVALID_ROOM_ID;
    }

    void User::setName(const std::string& name)
    {
        _name = name;
    }

    void User::setSession(const std::shared_ptr<Session>& session)
    {
        Channel::SetSession(session);

        session->RegisterOnClose([this](int r) {
            onSessionClose(r);
            });
    }

    void User::onSessionClose(int reason)
    {
        auto user = SAMPLE_SERVER().GetWorld()->FindUser(_userKey);

        SAMPLE_SERVER().GetRoomManager()->LeaveRoom(_enterRoomId, _userKey);
        SAMPLE_SERVER().GetWorld()->UnregistUser(_userKey);

        LOG_DEBUG(L"onClose, userKey:{}, reason:{}", _userKey, reason);
    }
}

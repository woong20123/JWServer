#include "User.h"

namespace jw
{
    User::User() : _userKey{ 0 }
    {}
    User::User(std::shared_ptr<Session>& session) : Channel(session), _userKey{ 0 }
    {}
    User::~User()
    {}

    void User::Initialize(std::shared_ptr<Session>& session, const std::string& name)
    {
        setSession(session);
        setName(name);
    }

    void User::Initialize(std::shared_ptr<Session>&& session, const std::string& name)
    {
        Initialize(session, name);
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

    void User::setName(const std::string& name)
    {
        _name = name;
    }

    void User::setSession(const std::shared_ptr<Session>& session)
    {
        Channel::SetSession(session);
    }
}

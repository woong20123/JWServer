#pragma once

#ifndef __JW_USER_H__
#define __JW_USER_H__
#include "Channel.h"
#include <string>

namespace jw
{
    class User : public Channel
    {
    public:
        User();
        User(std::shared_ptr<Session>& session);
        virtual ~User();

        void Initialize(std::shared_ptr<Session>& session, const std::string& name);
        void Initialize(std::shared_ptr<Session>&& session, const std::string& name);
        std::string_view GetName() const;

        void SetUserKey(const int64_t userId);
        int64_t GetUserKey() const;

    private:
        void setName(const std::string& name);
        void setSession(const std::shared_ptr<Session>& session);
        std::string _name;
        int64_t _userKey;
    };
}
#endif


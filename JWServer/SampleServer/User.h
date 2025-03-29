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
        static constexpr int64_t INVALID_USER_KEY = -1;

        User();
        User(std::shared_ptr<Session>& session);
        virtual ~User();

        void Initialize(std::shared_ptr<Session>& session, const std::string& name);
        void Initialize(std::shared_ptr<Session>&& session, const std::string& name);
        std::string_view GetName() const;

        void SetUserKey(const int64_t userId);
        int64_t GetUserKey() const;

        void SetEnterRoomId(const int64_t roomId);
        int64_t GetEnterRoomId() const;
        bool IsEnterRoom() const;

    private:
        // 세션 종료시 호출되는 함수 
        void onSessionClose(int reason);

        void setName(const std::string& name);
        void setSession(const std::shared_ptr<Session>& session);
        std::string _name;
        int64_t _userKey;
        int64_t _enterRoomId;
    };
}
#endif


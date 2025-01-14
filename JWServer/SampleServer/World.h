#pragma once
#ifndef __JW_WORLD_H__
#define __JW_WORLD_H__
#include <memory>
#include <unordered_map>
#include <queue>
#include <shared_mutex>
#include "Singleton.hpp"

namespace jw
{
    class User;
    class World
    {
        static constexpr size_t USER_LIST_MAX_SIZE = 5000;
    public:
        World();
        virtual ~World();

        void Initialize(size_t userListSize);
        bool RegistUser(std::shared_ptr<User> user);
        void UnregistUser(std::shared_ptr<User> user, const int64_t key);
        std::shared_ptr<User> FindUser(const int64_t userKey);
    private:

        std::shared_mutex	_userList_mutex;
        std::queue<int64_t> _issueBaseKey;
        std::unordered_map<int64_t, std::shared_ptr<User>> _userList;
        std::unordered_map<std::string, std::shared_ptr<User>> _userListByName;
    };
}

#endif // !__JW_WORLD_H__

#pragma once
#ifndef __JW_WORLD_H__
#define __JW_WORLD_H__
#include <memory>
#include <unordered_map>
#include <queue>
#include <shared_mutex>
#include "Serializer.h"

namespace jw
{
    enum SERIALIZER_TYPE
    {
        SERIALIZER_TYPE_NONE = 0,
        SERIALIZER_TYPE_WORLD,
        SERIALIZER_TYPE_ROOM,
        SERIALIZER_TYPE_MAX
    };

    enum REGITER_USER_RESULT
    {
        REGITER_USER_RESULT_NONE = 0,
        REGITER_USER_RESULT_SUCCESS,
        REGITER_USER_RESULT_DUPLICATE_NAME,
        REGITER_USER_RESULT_NOT_FIND_ISSUE_KEY,
        REGITER_USER_RESULT_MAX
    };

    class User;
    class Serializer;
    struct SerializerKey;
    class SerializeObject;
    class Packet;
    class World
    {
    public:
        static constexpr size_t USER_LIST_MAX_SIZE = 5000;
        World();
        ~World() = default;

        void Initialize(size_t userListSize);
        REGITER_USER_RESULT RegistUser(std::shared_ptr<User> user);
        void UnregistUser(const int64_t key);
        std::shared_ptr<User> FindUser(const int64_t userKey) const;
        void BroadcastPacket(Packet& packet);
        bool PostSO(std::shared_ptr<SerializeObject>& so);
        SerializerKey GetSerializerKey() const;

        void Shutdown();
    private:

        std::vector<int64_t> getAllUserKey() const;

        mutable std::shared_mutex	_userList_mutex;
        std::queue<int64_t> _issueBaseKey;
        std::unordered_map<int64_t, std::shared_ptr<User>> _userList;
        std::unordered_map<std::string, std::shared_ptr<User>> _userListByName;
        std::shared_ptr<Serializer> _serializer;
        std::unique_ptr<SerializerKey> _serializerKey;
    };
}

#endif // !__JW_WORLD_H__

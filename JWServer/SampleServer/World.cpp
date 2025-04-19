#include "World.h"
#include "User.h"
#include "TypeDefinition.h"
#include "Serializer.h"
#include "SerializerManager.h"
#include "Packet.h"
#include "TimerLauncher.h"
#include "Logger.h"

namespace jw
{
    World::World()
    {
    }
    World::~World()
    {
    }

    void World::Initialize(size_t userListSize)
    {
        for (int i = 1; i <= userListSize; ++i) {
            _issueBaseKey.push(i);
        }

        // WorldSerializer 등록 
        SerializerKey serializerKey{ SERIALIZER_TYPE_WORLD, SERIALIZER_MANAGER().MakeSerializeId(SERIALIZER_TYPE_WORLD) };
        _serializerKey = std::make_unique<SerializerKey>(serializerKey);
        _serializer = std::make_shared<Serializer>(serializerKey, TIMER_LAUNCHER().GetTickIntervalMilliSecond());
        SERIALIZER_MANAGER().RegistSerializer(serializerKey, _serializer);
    }

    REGITER_USER_RESULT World::RegistUser(std::shared_ptr<User> user)
    {
        // 이름이 추가 가능한지 체크
        WRITE_LOCK(_userList_mutex);
        std::string name = user->GetName().data();
        if (name.empty() || _userListByName.contains(name))
            return REGITER_USER_RESULT_DUPLICATE_NAME;

        if (_issueBaseKey.empty())
            return REGITER_USER_RESULT_NOT_FIND_ISSUE_KEY;

        const auto key = _issueBaseKey.front();
        _issueBaseKey.pop();

        _userList[key] = user;
        _userListByName[user->GetName().data()] = user;
        user->SetUserKey(key);

        return REGITER_USER_RESULT_SUCCESS;
    }
    void World::UnregistUser(const int64_t key)
    {
        WRITE_LOCK(_userList_mutex);

        if (!_userList.contains(key))
        {
            LOG_ERROR(L"UnregistUser not found user, key:{}", key);
            return;
        }

        std::shared_ptr<User> user = _userList[key];

        _userListByName.erase(user->GetName().data());
        _userList.erase(key);
        _issueBaseKey.push(key);
    }

    std::shared_ptr<User> World::FindUser(const int64_t userKey)
    {
        READ_LOCK(_userList_mutex);
        return _userList[userKey];
    }

    void World::BroadcastPacket(Packet& packet)
    {
        WRITE_LOCK(_userList_mutex);
        for (const auto& user : _userList)
        {
            if (user.second)
                user.second->Send(packet);
            else
                LOG_ERROR(L"BroadcastPacket user is nullptr");
        }
    }

    bool World::PostSO(std::shared_ptr<SerializeObject>& so)
    {
        _serializer->Post(so);
        return true;
    }

    SerializerKey World::GetSerializerKey() const
    {
        return *_serializerKey;
    }
}

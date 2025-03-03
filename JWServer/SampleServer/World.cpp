#include "World.h"
#include "User.h"
#include "TypeDefinition.h"
#include "Serializer.h"
#include "SerializerManager.h"
#include "Packet.h"

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
        _serializer = std::make_shared<Serializer>(serializerKey);
        SERIALIZER_MANAGER().RegistSerializer(serializerKey, _serializer);
    }

    bool World::RegistUser(std::shared_ptr<User> user)
    {
        // 이름이 추가 가능한지 체크
        WRITE_LOCK(_userList_mutex);
        std::string name = user->GetName().data();
        if (name.empty() || _userListByName.contains(name))
            return false;

        if (_issueBaseKey.empty())
            return false;

        const auto key = _issueBaseKey.front();
        _issueBaseKey.pop();

        _userList[key] = user;
        _userListByName[user->GetName().data()] = user;
        user->SetUserKey(key);

        return true;
    }
    void World::UnregistUser(std::shared_ptr<User> user, const int64_t key)
    {
        WRITE_LOCK(_userList_mutex);
        _userList.erase(key);
        _userListByName.erase(user->GetName().data());
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
            user.second->Send(packet);
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

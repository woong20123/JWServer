#include "ThreadManager.h"
#include "ThreadHelper.h"
#include "Logger.h"
#include "TypeDefinition.h"
#include <ranges>

namespace jw
{
    ThreadChecker::ThreadChecker() = default;
    ThreadChecker::~ThreadChecker() = default;

    void ThreadChecker::Initialize() {}
    void ThreadChecker::RunThread()
    {
        using namespace std::placeholders;

        auto t = ThreadHelper::MakeThreadAndGetInfo(L"ThreadChecker", _threadId, _UpdateExecutionFunc);
        t->SetExecution(std::bind(&ThreadChecker::execute, this, _1));
        GetThreadManager().AddThread(std::move(t));
    }

    void ThreadChecker::execute(std::stop_token stopToken)
    {
        LOG_INFO(L"ThreadChecker started, tid:{}", std::this_thread::get_id());
        std::this_thread::sleep_for(std::chrono::seconds(10));

        while (true)
        {
            if (stopToken.stop_requested())
            {
                LOG_ERROR(L"ThreadChecker stopped, tid:{}", std::this_thread::get_id());
                break;
            }

            _UpdateExecutionFunc();

            std::vector<Thread::ThreadId> threadIds;
            GetThreadManager().GetThreadIds(threadIds);
            for (const auto& threadId : threadIds)
            {
                if (GetThreadManager().CheckFrozenThread(threadId))
                {
                    LOG_ERROR(L"Frozen thread detected, ID: {}", threadId);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(10)); // Check every 10 seconds
        }
        onCloseExecute();
    }

    void ThreadChecker::onCloseExecute()
    {
        if (_threadId != std::thread::id())
        {
            GetThreadManager().RemoveThread(_threadId);
        }
        else
        {
            LOG_ERROR(L"Thread does not exist, threadId:{}", _threadId);
        }

        _UpdateExecutionFunc = nullptr;
    }

    ThreadManager::ThreadManager()
    {}
    ThreadManager::~ThreadManager()
    { }

    void ThreadManager::SetThreadFrozenTime(const time_t frozenTime)
    {
        _threadFrozenCheckTime = frozenTime;
    }

    bool ThreadManager::AddThread(std::unique_ptr<Thread>&& thread)
    {
        WRITE_LOCK(_mutex);
        if (_threads.contains(thread->GetThraedId()))
        {
            LOG_ERROR(L"Thread with ID {} already exists.", thread->GetThraedId());
            return false;
        }

        LOG_INFO(L"ThreadManager Add Thread({})", thread->GetFullName());
        _threads.emplace(thread->GetThraedId(), std::move(thread));
        return true;
    }
    bool ThreadManager::ExistsThread(const Thread::ThreadId& threadId) const
    {
        READ_LOCK(_mutex);
        return _threads.contains(threadId);
    }

    void ThreadManager::RemoveThread(const Thread::ThreadId& threadId)
    {
        WRITE_LOCK(_mutex);
        if (const auto it = _threads.find(threadId);
            it != _threads.end())
        {
            LOG_INFO(L"Thread({}) removed successfully.", it->second->GetFullName());
            _threads.erase(it);
        }
        else
        {
            LOG_ERROR(L"Thread({}) does not exist.", threadId);
        }
    }

    void ThreadManager::StopThread(const Thread::ThreadId& threadId)
    {
        WRITE_LOCK(_mutex);
        if (const auto it = _threads.find(threadId);
            it != _threads.end())
        {
            it->second->Stop();
            LOG_INFO(L"Thread({}) stopped", it->second->GetFullName());
        }
        else
        {
            LOG_ERROR(L"Thread({}) does not exist.", threadId);
        }
    }

    bool ThreadManager::CheckFrozenThread(const Thread::ThreadId& threadId) const
    {
        std::wstring threadName{ L"" };
        time_t lastExecutionTime{ 0 };
        time_t currentTime = std::time(nullptr);
        bool findThread{ false };
        {
            READ_LOCK(_mutex);
            if (_threads.contains(threadId))
            {
                lastExecutionTime = _threads.at(threadId)->GetLastExecutionTime();
                threadName = _threads.at(threadId)->GetFullName();
                findThread = true;
            }
        }

        if (!findThread)
        {
            LOG_ERROR(L"Thread does not exist. threadName:{}", threadName);
            return false;
        }

        if (0 == lastExecutionTime)
        {
            return false;
        }

        const auto frozenTime = currentTime - lastExecutionTime;
        if (frozenTime > _threadFrozenCheckTime) // 60 seconds threshold
        {
            LOG_FETAL(L"Thread {} is frozen, last execution time: {}, current time: {}, _threadFrozenTime: {}, frozenTime: {}", threadName, lastExecutionTime, currentTime, _threadFrozenCheckTime, frozenTime);
            return true;
        }

        return false;
    }

    void ThreadManager::GetThreadIds(std::vector<Thread::ThreadId>& threadIds) const
    {
        READ_LOCK(_mutex);
        threadIds.reserve(_threads.size());

        for (const auto& id : _threads | std::views::keys)
        {
            threadIds.push_back(id);
        }
    }

    ThreadManager& GetThreadManager()
    {
        return ThreadManager::GetInstance();
    }
}

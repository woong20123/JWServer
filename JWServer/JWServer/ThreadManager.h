#pragma once
#ifndef __JW_THREAD_MANAGER_H__
#define __JW_THREAD_MANAGER_H__
#include "Singleton.hpp"
#include "Thread.h"
#include <unordered_map>
#include <shared_mutex>
#include <functional>

namespace jw
{
    class ThreadChecker
    {
    public:
        ThreadChecker();
        ~ThreadChecker();
        void Initialize();
        void RunThread();
    private:
        void execute(std::stop_token stopToken);
        void onClose();

        std::thread::id             _threadId;
        std::function<void()>       _UpdateExecutionFunc;
    };

    class ThreadManager : public Singleton<ThreadManager>
    {
    public:
        using ThreadContainer = std::unordered_map<Thread::ThreadId, std::unique_ptr<Thread>>;
        constexpr static time_t DEFAULT_THREAD_FROZEN_TIME = 60; // 스레드가 멈춘 것으로 간주되는 시간 (초 단위)

        void SetThreadFrozenTime(const time_t frozenTime);

        bool AddThread(std::unique_ptr<Thread>&& thread);
        bool ExistsThread(const Thread::ThreadId& threadId) const;
        void RemoveThread(const Thread::ThreadId& threadId);
        bool CheckFrozenThread(const Thread::ThreadId& threadId) const;
        void GetThreadIds(std::vector<Thread::ThreadId>& ids) const;
    protected:
        ThreadManager();
        ~ThreadManager();
    private:
        mutable std::shared_mutex _mutex;
        ThreadContainer _threads;
        time_t _threadFrozenCheckTime{ DEFAULT_THREAD_FROZEN_TIME }; // 스레드가 멈춘 것으로 간주되는 시간 (초 단위)

        friend class Singleton<ThreadManager>;
    };

    ThreadManager& GetThreadManager();
}
#endif // !__JW_THREAD_MANAGER_H__


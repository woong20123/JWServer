#pragma once
#ifndef __JW_THREAD_HELPER_H__
#define __JW_THREAD_HELPER_H__
#include <string>
#include <thread>
#include <functional>
#include <memory>
#include "Thread.h"
namespace jw
{
    class Thread;
    struct ThreadInfoRef
    {
        std::wstring threadName;
        std::thread::id& threadId;
        std::function<void()>& updateExecutionFunc;
    };

    class ThreadHelper
    {
    public:
        template<typename Func, typename... Args>
        static std::unique_ptr<Thread> MakeThreadAndGetInfo(std::wstring_view threadName, Func&& func, Args... args)
        {
            auto t = std::make_unique<Thread>();
            t->Initialize(threadName.data());
            t->SetExecution(std::forward<Func>(func), std::forward<Args>(args)...);
            return std::move(t);
        }
    };
}

#endif // !__JW_THREAD_HELPER_H_
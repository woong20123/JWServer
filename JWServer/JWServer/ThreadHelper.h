#pragma once
#ifndef __JW_THREAD_HELPER_H__
#define __JW_THREAD_HELPER_H__
#include <string>
#include <thread>
#include <functional>
#include <memory>
namespace jw
{
    class Thread;
    class ThreadHelper
    {
    public:
        static std::unique_ptr<Thread> MakeThreadAndGetInfo(const std::wstring_view threadName, std::thread::id& threadId, std::function<void()>& _UpdateExecutionFunc);
    };
}

#endif // !__JW_THREAD_HELPER_H__

#pragma once
#ifndef __JW_THREAD_H__
#define __JW_THREAD_H__
#include <thread>
#include <string>

namespace jw
{
    class Thread
    {
    public:
        using ThreadHandle = std::jthread;
        using ThreadId = std::jthread::id;
        using NameType = std::wstring;

        Thread();
        ~Thread();

        Thread(Thread& other) = delete; // 복사 생성자 금지
        const Thread& operator=(const Thread& other) = delete; // 복사 대입 연산자 금지

        void Initialize(const NameType& name);
        void Initialize(const NameType& name, std::jthread&& thread);

        template<typename Func, typename... Args>
        void SetExecution(Func&& func, Args... args)
        {
            if (_thread.joinable())
            {
                std::runtime_error("Thread is already running. Cannot set execution function again.");
            }
            _thread = std::jthread(std::forward<Func>(func), std::forward<Args>(args)...);
        }

        const NameType& GetName() const { return _name; }
        const NameType GetFullName() const;
        time_t GetLastExecutionTime() const { return _lastExecution; }
        void UpdateLastExecutionTime();
        const ThreadId GetThraedId() const;
        void Stop();
    private:
        std::jthread _thread;
        NameType _name;
        time_t _lastExecution;
    };
}

#endif // __JW_THREAD_H__
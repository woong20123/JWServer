#include "Thread.h"
#include "Thread.h"
#include "Logger.h"

namespace jw
{
    Thread::Thread() : _name{ L"" }, _lastExecution(0)
    {
    }

    Thread::~Thread()
    {
        if (_thread.joinable())
        {
            _thread.request_stop();
            _thread.join();
        }
    }

    void Thread::Initialize(const string& name)
    {
        _name = name;
    }

    void Thread::Initialize(const string& name, std::jthread&& thread)
    {
        _name = name;
        _thread = std::move(thread);
    }

    void Thread::UpdateLastExecutionTime()
    {
        _lastExecution = std::time(nullptr);
    }

    const Thread::ThreadId Thread::GetThraedId() const
    {
        if (_thread.joinable())
        {
            return _thread.get_id();
        }
        return ThreadId{};
    }

    const Thread::string Thread::GetFullName() const
    {
        return std::format(L"{}-{}", _name, _thread.get_id());
    }
}
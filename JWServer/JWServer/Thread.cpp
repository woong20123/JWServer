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
        }
    }

    void Thread::Initialize(const NameType& name)
    {
        _name = name;
    }

    void Thread::Initialize(const NameType& name, std::jthread&& thread)
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

    void Thread::Stop()
    {
        _thread.request_stop();
    }

    const Thread::NameType Thread::GetFullName() const
    {
        return std::format(L"name:{}, id:{}", _name, _thread.get_id());
    }
}
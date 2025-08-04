#include "ThreadHelper.h"
#include "Thread.h"

namespace jw
{
    std::unique_ptr<Thread> ThreadHelper::MakeThreadAndGetInfo(const std::wstring_view threadName, std::thread::id& threadId, std::function<void()>& _UpdateExecutionFunc)
    {
        auto t = std::make_unique<Thread>();
        t->Initialize(threadName.data());
        threadId = t->GetThraedId();
        _UpdateExecutionFunc = [tPtr = t.get()]() { tPtr->UpdateLastExecutionTime(); };
        return t;
    }
}
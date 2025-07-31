#include "IOWorker.h"
#include "Logger.h"
#include "AsyncObject.h"
#include "Thread.h"
#include "ThreadManager.h"
#include <functional>

namespace jw
{
    IOWorker::IOWorker() : _iocpHandle{ INVALID_HANDLE_VALUE }, _UpdateExecutionFunc{ nullptr }
    {}

    IOWorker::~IOWorker()
    {
        _iocpHandle = INVALID_HANDLE_VALUE;

        /*for (auto& t : _threads)
            t.join();*/
    }

    void IOWorker::Initialize(HANDLE iocpHandle)
    {
        _iocpHandle = iocpHandle;
        if (INVALID_HANDLE_VALUE == _iocpHandle)
        {
            LOG_FETAL(L"INVALID iocpHandle");
        }
    }

    void IOWorker::RunThread()
    {
        using namespace std::placeholders;

        auto t = std::make_unique<Thread>();
        t->Initialize(L"IOWorker");
        t->SetExecution(std::bind(&IOWorker::execute, this, _1));

        _threadId = t->GetThraedId();
        _UpdateExecutionFunc = [tPtr = t.get()]() { tPtr->UpdateLastExecutionTime(); };

        GetThreadManager().AddThread(std::move(t));
    }

    void IOWorker::execute(std::stop_token stopToken)
    {
        BOOL result{ FALSE };
        unsigned long numOfBytes{ 0 };
        AsyncObject* object{ nullptr };
        AsyncContext* context{ nullptr };
        constexpr int MAX_WAIT = 1000;

        while (true)
        {
            result = ::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, reinterpret_cast<ULONG_PTR*>(&object), reinterpret_cast<OVERLAPPED**>(&context), MAX_WAIT);

            if (stopToken.stop_requested())
            {
                LOG_ERROR(L"IOWorker stopped, tid:{}", std::this_thread::get_id());
                break;
            }

            _UpdateExecutionFunc();

            if (result)
            {
                if (!object->HandleEvent(context, numOfBytes))
                {
                    if (object) {
                        LOG_ERROR(L"HandleEvent fail, tid:{}, eventId:{}", std::this_thread::get_id(), object->GetAsyncObjectId());
                        object->HandleFailedEvent(context, numOfBytes);
                    }
                }
            }
            else
            {
                auto err = ::WSAGetLastError();
                if (WAIT_TIMEOUT != err) {
                    LOG_FETAL(L"GetQueuedCompletionStatus fail, error:{}", err);
                    object->HandleFailedEvent(context, numOfBytes);
                }
            }
        }

        onClose();
    }

    void IOWorker::onClose()
    {
        if (GetThreadManager().ExistsThread(_threadId))
        {
            GetThreadManager().RemoveThread(_threadId);
        }

        _UpdateExecutionFunc = nullptr;
    }
}
#include "IOWorker.h"
#include "Logger.h"
#include "AsyncIOObject.h"

namespace jw
{
    IOWorker::IOWorker() : _iocpHandle{ INVALID_HANDLE_VALUE }
    {}

    IOWorker::~IOWorker()
    {}

    void IOWorker::Initialize(HANDLE iocpHandle)
    {
        _iocpHandle = iocpHandle;
        if (INVALID_HANDLE_VALUE == _iocpHandle)
        {
            LOG_FETAL(L"INVALID iocpHandle");
        }
    }

    void IOWorker::RunThreads(uint16_t workerThreadCount)
    {
        for (int i = 0; i < workerThreadCount; ++i)
            _threads.emplace_back(&IOWorker::execute, this);
    }

    void IOWorker::execute()
    {
        prepare();

        BOOL result{ FALSE };
        unsigned long numOfBytes{ 0 };
        AsyncIOObject* object{ nullptr };
        AsyncContext* context{ nullptr };
        constexpr int MAX_WAIT = 1000;

        while (true)
        {
            result = ::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, reinterpret_cast<ULONG_PTR*>(&object), reinterpret_cast<OVERLAPPED**>(&context), MAX_WAIT);

            if (result)
            {
                if (!object->HandleEvent(context, numOfBytes))
                {
                    LOG_FETAL(L"HandleEvent fail, eventId:{}", object->GetAsyncObjectId());
                    object->HandleFailedEvent(context, numOfBytes);
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
    }
}

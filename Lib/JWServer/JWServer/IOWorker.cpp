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
            LOG_FETAL_STRING(L"INVALID iocpHandle");
        }

    }


    void IOWorker::RunThread()
    {
        _thread = std::thread(&IOWorker::execute, this);
    }

    void IOWorker::execute()
    {
        prepare();

        BOOL result{ FALSE };
        unsigned long numOfBytes{ 0 };
        AsyncIOObject* object{ nullptr };
        OVERLAPPED* context{ nullptr };
        constexpr int MAX_WAIT = 1000;

        while (true)
        {
            result = ::GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, (ULONG_PTR*)&object, &context, MAX_WAIT);

            if (result)
            {
                if (!object->HandleEvent(context, numOfBytes))
                {
                    LOG_FETAL(L"HandleEvent fail, eventId:{}", object->GetID());
                }
            }
            else
            {
                auto err = ::WSAGetLastError();
                if(WAIT_TIMEOUT != err)
                    LOG_FETAL(L"GetQueuedCompletionStatus fail, error:{}", err);
            }
        }
    }
}

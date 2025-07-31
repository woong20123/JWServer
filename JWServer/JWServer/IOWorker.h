#pragma once
#ifndef __JW_IO_WORKER_H__
#define __JW_IO_WORKER_H__
#include <thread>
#include <WinSock2.h>
#include <vector>
#include <functional>

namespace jw
{
    class IOWorker
    {
    public:
        IOWorker();
        virtual ~IOWorker();
        void Initialize(HANDLE iocpHandle);
        void RunThread();
    protected:
        void onClose();
        void execute(std::stop_token stopToken);
    private:
        std::thread::id _threadId;
        HANDLE          _iocpHandle;
        std::function<void()> _UpdateExecutionFunc;
    };
}
#endif
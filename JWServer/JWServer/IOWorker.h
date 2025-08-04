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
        void SetId(int32_t workerThreadId) { _id = workerThreadId; }
    protected:
        void onCloseExecute();
        void execute(std::stop_token stopToken);
    private:
        int32_t             _id;
        std::thread::id     _threadId;
        HANDLE              _iocpHandle;
        std::function<void()> _UpdateExecutionFunc;
    };
}
#endif
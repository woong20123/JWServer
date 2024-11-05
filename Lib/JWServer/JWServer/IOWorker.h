#pragma once
#ifndef __JW_IO_WORKER_H__
#define __JW_IO_WORKER_H__
#include <thread>
#include <WinSock2.h>

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
        virtual void prepare() {};
        virtual void execute();
    private:
        std::thread	        _thread;
        HANDLE              _iocpHandle;
    };
}
#endif
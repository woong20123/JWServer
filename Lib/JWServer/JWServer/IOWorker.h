#pragma once
#ifndef __JW_IO_WORKER_H__
#define __JW_IO_WORKER_H__
#include <thread>
#include <WinSock2.h>
#include <vector>

namespace jw
{

    class IOWorker
    {
    public:
        IOWorker();
        virtual ~IOWorker();
        void Initialize(HANDLE iocpHandle);
        void RunThreads(uint16_t workerThreadCount);
    protected:
        virtual void prepare() {};
        void execute();
    private:
        std::vector<std::thread>    _threads;
        HANDLE                      _iocpHandle;
    };
}
#endif
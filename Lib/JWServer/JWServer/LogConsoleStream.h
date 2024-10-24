#pragma once
#ifndef __JW_LOG_CONSOLE_STREAM_H_
#define __JW_LOG_CONSOLE_STREAM_H_
#include "LogStream.h"
namespace jw
{
    class LogConsoleStream : public LogStream
    {
    public:
        LogConsoleStream();
        ~LogConsoleStream() override;
        void Write(const std::shared_ptr<LogBuffer>& logBuffer) override;
        void Flush() override;
    private:
        void initBuffer();

        struct Impl;
        std::unique_ptr<Impl> _pImpl;
    };
}
#endif
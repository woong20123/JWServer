#pragma once
#ifndef __JW_LOG_FILE_STREAM_H__
#define __JW_LOG_FILE_STREAM_H__
#include "LogStream.h"

namespace jw
{
    class LogFileStream : public LogStream
    {
    public:
        LogFileStream(const char* path, const char* filename);
        ~LogFileStream() override;
        void Write(const std::shared_ptr<LogBuffer>& logBuffer) override;
        void Flush() override;
    private:
        void initBuffer();
        void makeFileName();
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
    };
}
#endif // __JW_LOG_FILE_STREAM_H_
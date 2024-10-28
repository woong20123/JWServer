#pragma once
#ifndef __JW_LOG_FILE_STREAM_H__
#define __JW_LOG_FILE_STREAM_H__
#include "LogStream.h"
#include <string_view>

namespace jw
{
    class LogFileStream : public LogStream
    {
    public:
        LogFileStream(const wchar_t* path, const wchar_t* filename);
        ~LogFileStream() override;

        void Initialize() override;
        void Write(const std::shared_ptr<LogBuffer>& logBuffer) override;
        void Flush() override;
    private:
        void initBuffer();
        void makeFileName();
        void makeFolder(std::wstring_view path);
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
    };
}
#endif // __JW_LOG_FILE_STREAM_H_
#pragma once
#ifndef __JW_LOG_FILE_STREAM_H__
#define __JW_LOG_FILE_STREAM_H__
#include "LogStream.h"
#include "LogBuffer.h"
#include <string_view>

namespace jw
{
    class LogFileStream : public LogStream
    {
    public:
        static constexpr size_t DEFUALT_BUFFER_SIZE = 204800;

        LogFileStream(const wchar_t* path, const wchar_t* filename, size_t bufferSize = DEFUALT_BUFFER_SIZE);
        ~LogFileStream() override;

        void Initialize() override;
        void Write(const std::shared_ptr<LogBuffer>& logBuffer) override;
        void Flush() override;
    private:
        void initBuffer();
        inline const size_t remainBuffer() { return _bufferSize - _bufferPos; }
        void makeFileName();
        void makeFolder(std::wstring_view path);

        LogBuffer::BufferType * _logBuffer{ nullptr };
        size_t                  _bufferPos;
        size_t                  _bufferSize;
        std::wstring            _processname;
        std::wstring            _path;
        std::wstring            _fullFileName;
        size_t                  _fileNameTick;
    };
}
#endif // __JW_LOG_FILE_STREAM_H_
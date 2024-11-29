#pragma once
#ifndef __JW_LOG_CONSOLE_STREAM_H_
#define __JW_LOG_CONSOLE_STREAM_H_
#include "LogStream.h"
#include "LogBuffer.h"
namespace jw
{
    class LogConsoleStream : public LogStream
    {
    public:
        static constexpr size_t DEFUALT_BUFFER_SIZE = 204800;

        LogConsoleStream(size_t bufferSize = DEFUALT_BUFFER_SIZE);
        ~LogConsoleStream() override;
        void Write(const std::shared_ptr<LogBuffer>& logBuffer) override;
        void Flush() override;
    private:
        void initBuffer();
        inline const size_t remainBuffer() { return _bufferSize - _bufferPos; }

        LogBuffer::BufferType * _logBuffer;
        size_t _bufferPos;
        size_t _bufferSize;
    };
}
#endif
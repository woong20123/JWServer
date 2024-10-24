#include "LogConsoleStream.h"
#include "LogBuffer.h"

namespace jw
{
    struct LogConsoleStream::Impl
    {
        static constexpr size_t BUFFER_SIZE = 409600;
        Impl() : bufferPos{ 0 }
        {}
        LogBuffer::BufferType logBuffer[BUFFER_SIZE] = { 0, };
        size_t bufferPos;
    };

    LogConsoleStream::LogConsoleStream() : _pImpl{ std::make_unique<LogConsoleStream::Impl>() }
    {}

    LogConsoleStream::~LogConsoleStream()
    {}

    void LogConsoleStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
    {
        const auto prefixLen = strlen(logBuffer->GetPrefix());
        const auto msgLen = strlen(logBuffer->GetMsg());
        size_t totalMsgLen = prefixLen + msgLen;

        if (Impl::BUFFER_SIZE <= _pImpl->bufferPos + totalMsgLen)
            Flush();

        // 메시지 복사
        strncpy_s(&_pImpl->logBuffer[_pImpl->bufferPos], Impl::BUFFER_SIZE - _pImpl->bufferPos, logBuffer->GetPrefix(), prefixLen);
        _pImpl->bufferPos += prefixLen;
        strncpy_s(&_pImpl->logBuffer[_pImpl->bufferPos], Impl::BUFFER_SIZE - _pImpl->bufferPos, logBuffer->GetMsg(), msgLen);
        _pImpl->bufferPos += msgLen;
    }

    void LogConsoleStream::Flush()
    {
        printf_s("%s", _pImpl->logBuffer);
        initBuffer();
    }

    void LogConsoleStream::initBuffer()
    {
        _pImpl->bufferPos = 0;
        _pImpl->logBuffer[0] = '\0';
    }
}

#include "LogConsoleStream.h"
#include "LogBuffer.h"
#include "TypeDefinition.h"

namespace jw
{
    struct LogConsoleStream::Impl
    {
        static constexpr size_t DEFUALT_BUFFER_SIZE = 204800;
        Impl(size_t bufferSize = DEFUALT_BUFFER_SIZE) : logBuffer{ nullptr }, bufferPos{ 0 }, bufferSize{ bufferSize }
        {
            logBuffer = new LogBuffer::BufferType[bufferSize];
        }
        ~Impl()
        {
            if (logBuffer)
            {
                delete[] logBuffer;
                logBuffer = nullptr;
            }

        }
        LogBuffer::BufferType* logBuffer;
        size_t bufferPos;
        size_t bufferSize;

        inline const size_t remainBuffer() { return bufferSize - bufferPos; }
    };

    LogConsoleStream::LogConsoleStream() : _pImpl{ std::make_unique<LogConsoleStream::Impl>() }
    {}

    LogConsoleStream::~LogConsoleStream()
    {}

    void LogConsoleStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
    {
        const auto prefixLen = STRLEN(logBuffer->GetPrefix());
        const auto msgLen = STRLEN(logBuffer->GetMsg());
        const auto suffixLen = STRLEN(logBuffer->GetSuffix());
        size_t totalMsgLen = prefixLen + msgLen + suffixLen;

        if (_pImpl->bufferSize <= _pImpl->bufferPos + totalMsgLen)
            Flush();

        // 메시지 복사
        STRNCPY(&_pImpl->logBuffer[_pImpl->bufferPos], _pImpl->remainBuffer(), logBuffer->GetPrefix(), prefixLen);
        _pImpl->bufferPos += prefixLen;
        STRNCPY(&_pImpl->logBuffer[_pImpl->bufferPos], _pImpl->remainBuffer(), logBuffer->GetMsg(), msgLen);
        _pImpl->bufferPos += msgLen;
        STRNCPY(&_pImpl->logBuffer[_pImpl->bufferPos], _pImpl->remainBuffer(), logBuffer->GetSuffix(), suffixLen);
        _pImpl->bufferPos += suffixLen;
    }

    void LogConsoleStream::Flush()
    {
        wprintf_s(L"%s", _pImpl->logBuffer);
        initBuffer();
    }

    void LogConsoleStream::initBuffer()
    {
        _pImpl->bufferPos = 0;
        _pImpl->logBuffer[0] = '\0';
    }
}

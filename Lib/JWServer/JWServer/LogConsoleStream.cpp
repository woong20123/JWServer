#include "LogConsoleStream.h"

#include "TypeDefinition.h"

namespace jw
{
    LogConsoleStream::LogConsoleStream(size_t bufferSize) : _logBuffer{ nullptr }, _bufferPos{ 0 }, _bufferSize{ bufferSize }
    {
        _logBuffer = new LogBuffer::BufferType[bufferSize];
    }

    LogConsoleStream::~LogConsoleStream()
    {
        if (_logBuffer)
        {
            delete[] _logBuffer;
            _logBuffer = nullptr;
        }
    }

    void LogConsoleStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
    {
        const auto prefixLen = STRLEN(logBuffer->GetPrefix());
        const auto msgLen = STRLEN(logBuffer->GetMsg());
        const auto lineBreakLen = STRLEN(logBuffer->GetLineBreak());
        size_t totalMsgLen = prefixLen + msgLen + lineBreakLen;

        if (_bufferSize <= _bufferPos + totalMsgLen)
            Flush();

        // 메시지 복사
        STRNCPY(&_logBuffer[_bufferPos], remainBuffer(), logBuffer->GetPrefix(), prefixLen);
        _bufferPos += prefixLen;
        STRNCPY(&_logBuffer[_bufferPos], remainBuffer(), logBuffer->GetMsg(), msgLen);
        _bufferPos += msgLen;
        STRNCPY(&_logBuffer[_bufferPos], remainBuffer(), logBuffer->GetLineBreak(), lineBreakLen);
        _bufferPos += lineBreakLen;
    }

    void LogConsoleStream::Flush()
    {
        wprintf_s(L"%s", _logBuffer);
        initBuffer();
    }

    void LogConsoleStream::initBuffer()
    {
        _bufferPos = 0;
        _logBuffer[0] = '\0';
    }
}

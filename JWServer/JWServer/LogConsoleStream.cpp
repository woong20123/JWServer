#include "LogConsoleStream.h"

#include "TypeDefinition.h"

namespace jw
{
    LogConsoleStream::LogConsoleStream(size_t bufferSize) : _streamBuffer{ nullptr }, _streamBufferPos{ 0 }, _streamBufferSize{ bufferSize }
    {
        _streamBuffer = new LogBuffer::BufferType[bufferSize];
        ::memset(_streamBuffer, 0x00, sizeof(LogBuffer::BufferType) * bufferSize);
    }

    LogConsoleStream::~LogConsoleStream()
    {
        if (_streamBuffer)
        {
            delete[] _streamBuffer;
            _streamBuffer = nullptr;
        }
    }

    void LogConsoleStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
    {
        const auto prefixLen = STRLEN(logBuffer->GetPrefix());
        const auto msgLen = STRLEN(logBuffer->GetMsg());
        const auto lineBreakLen = STRLEN(logBuffer->GetLineBreak());
        size_t totalMsgLen = prefixLen + msgLen + lineBreakLen;

        if (_streamBufferSize <= _streamBufferPos + totalMsgLen)
            Flush();

        // 메시지 복사
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetPrefix(), prefixLen);
        _streamBufferPos += prefixLen;
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetMsg(), msgLen);
        _streamBufferPos += msgLen;
        STRNCPY(&_streamBuffer[_streamBufferPos], remainBuffer(), logBuffer->GetLineBreak(), lineBreakLen);
        _streamBufferPos += lineBreakLen;
    }

    void LogConsoleStream::Flush()
    {
        wprintf_s(L"%s", _streamBuffer);
        initBuffer();
    }

    void LogConsoleStream::initBuffer()
    {
        _streamBufferPos = 0;
        _streamBuffer[0] = '\0';
    }
}

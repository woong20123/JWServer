#include "LogFileStream.h"
#include "LogBuffer.h"
#include <string>
#include <windows.h>

namespace jw
{

    struct LogFileStream::Impl
    {
        static constexpr size_t BUFFER_SIZE = 409600;

        Impl(const char* path, const char* filename) : path{ path }, filename{ filename }, bufferPos{ 0 }
        {}
        LogBuffer::BufferType logBuffer[BUFFER_SIZE] = { 0, };
        size_t bufferPos;
        std::string filename;
        std::string path;
    };

    LogFileStream::LogFileStream(const char* path, const char* filename) : _pImpl{ std::make_unique<LogFileStream::Impl>(path, filename) }
    {}

    LogFileStream::~LogFileStream()
    {}

    void LogFileStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
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

    void LogFileStream::Flush()
    {
        HANDLE h{ INVALID_HANDLE_VALUE };
        constexpr int TRY_COUNT = 20;
        for (int i = 1; i < TRY_COUNT; ++i)
        {
            if (INVALID_HANDLE_VALUE != (h = CreateFileA(_pImpl->filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)))
                break;
            Sleep(100);
        }

        if (INVALID_HANDLE_VALUE == h)
        {
            _pImpl->bufferPos = 0;
            return;
        }

        DWORD written = 0;
        LONG highPos = 0;
        SetFilePointer(h, 0L, &highPos, FILE_END);
        WriteFile(h, _pImpl->logBuffer, (DWORD)_pImpl->bufferPos, &written, NULL);
        CloseHandle(h);

        initBuffer();
    }
    void LogFileStream::initBuffer()
    {
        _pImpl->bufferPos = 0;
        _pImpl->logBuffer[0] = '\0';
    }
    void LogFileStream::makeFileName()
    {

    }
}
#include "LogFileStream.h"
#include "LogBuffer.h"
#include "TypeDefinition.h"
#include <string>
#include <windows.h>
#include <filesystem>

namespace jw
{

    struct LogFileStream::Impl
    {
        static constexpr size_t DEFUALT_BUFFER_SIZE = 204800;

        Impl(const wchar_t* path, const wchar_t* processName, size_t bufferSize = DEFUALT_BUFFER_SIZE)
            : path{ path }, processname{ processName }, bufferPos{ 0 }, fileNameTick{ 0 }, bufferSize{ bufferSize }
        {
            logBuffer = new LogBuffer::BufferType[bufferSize];
        }
        ~Impl()
        {
            if (logBuffer) {
                delete[] logBuffer;
                logBuffer = nullptr;
            }
        }
        LogBuffer::BufferType* logBuffer{ nullptr };
        size_t bufferPos;
        size_t bufferSize;
        std::wstring processname;
        std::wstring path;
        std::wstring fullFileName;
        size_t fileNameTick;

        inline const size_t remainBuffer() { return bufferSize - bufferPos; }
    };

    LogFileStream::LogFileStream(const wchar_t* path, const wchar_t* filename) : _pImpl{ std::make_unique<LogFileStream::Impl>(path, filename) }
    {}

    LogFileStream::~LogFileStream()
    {}

    void LogFileStream::Initialize() {
        if (!std::filesystem::is_directory(_pImpl->path))
            makeFolder(_pImpl->path);
    }

    void LogFileStream::Write(const std::shared_ptr<LogBuffer>& logBuffer)
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

    void LogFileStream::Flush()
    {
        makeFileName();
        HANDLE h{ INVALID_HANDLE_VALUE };
        constexpr int TRY_COUNT = 20;
        for (int i = 1; i < TRY_COUNT; ++i)
        {
            if (INVALID_HANDLE_VALUE != (h = CreateFileW(_pImpl->fullFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)))
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
        const auto pos = SetFilePointer(h, 0L, &highPos, FILE_END);

        int result{ 0 };
        if (pos == 0)
        {
            // 파일이 처음 생성되면 유니코드 표기 기록
            unsigned short mark = 0xFEFF;
            result = ::WriteFile(h, &mark, sizeof(mark), &written, NULL);
        }

        result = ::WriteFile(h, _pImpl->logBuffer, (DWORD)_pImpl->bufferPos * sizeof(wchar_t), &written, NULL);
        ::CloseHandle(h);

        initBuffer();
    }
    void LogFileStream::initBuffer()
    {
        _pImpl->bufferPos = 0;
        _pImpl->logBuffer[0] = '\0';
    }

    void LogFileStream::makeFileName()
    {
        time_t now = time(NULL);
        struct tm tmNow;
        localtime_s(&tmNow, &now);
        const auto timeTick = (tmNow.tm_year + 1900) * 1000000 + (tmNow.tm_mon + 1) * 10000 + tmNow.tm_mday * 100 + tmNow.tm_hour;

        if (_pImpl->fileNameTick != timeTick)
        {
            auto hour = timeTick % 100;
            _pImpl->fullFileName.clear();
            _pImpl->fullFileName.append(_pImpl->path);
            _pImpl->fullFileName.append(_pImpl->processname);
            _pImpl->fullFileName.append(std::format(L"({})_{}-{:02}.log", ::GetCurrentProcessId(), timeTick / 100, hour));
            _pImpl->fileNameTick = timeTick;
        }
    }

    void LogFileStream::makeFolder(std::wstring_view path)
    {
        if (std::filesystem::create_directory(path))
        {
            std::wcout << L"폴더 생성 완료 : " << path << std::endl;
        }
        else
        {
            ASSERT_WITH_MSG(false, std::format(L"폴더 생성 실패 : {}", path));
        }
    }
}
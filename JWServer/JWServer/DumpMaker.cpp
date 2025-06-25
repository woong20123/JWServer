#include "DumpMaker.h"
#include "TypeDefinition.h"
#include <DbgHelp.h>
#include <filesystem>
#include <functional>
#include <ranges>

#pragma comment(lib, "Dbghelp.lib")

namespace jw
{
    void DumpMaker::Regist()
    {
        doRegist();
    }


    void DumpMaker::UnRegist()
    {
        doUnRegist();
    }


    void WindowDumpMaker::doRegist()
    {
        auto unhandledExceptionFilterFunc = [](EXCEPTION_POINTERS* pExceptionInfo)->LONG {
            WindowDumpMaker::unhandledExceptionFilter(pExceptionInfo);
            return EXCEPTION_EXECUTE_HANDLER; // 덤프 파일 경로 지정
            };

        _oldFilter = ::SetUnhandledExceptionFilter(unhandledExceptionFilterFunc);
    }

    std::wstring WindowDumpMaker::getApplicationName()
    {
        wchar_t applicationPath[MAX_PATH] = {};
        if (!GetModuleFileNameW(nullptr, applicationPath, MAX_PATH)) {
            return L"unknown";
        }

        std::wstring strApplicationPath{ applicationPath };
        std::wstring strApplicationName{ L"unknown" };
        size_t startPos = strApplicationPath.rfind(L'\\');
        size_t endPos = strApplicationPath.rfind(L'.');
        if (startPos != std::wstring::npos && endPos != std::wstring::npos && startPos < endPos) {
            strApplicationName = strApplicationPath.substr(startPos + 1, endPos - startPos);
        }

        return strApplicationName;
    }

    void WindowDumpMaker::unhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo)
    {
        constexpr const wchar_t* path = L"log\\dump\\";

        std::error_code ec;
        if (!std::filesystem::is_directory(path, ec))
        {
            std::filesystem::create_directory(path, ec);
            std::wcout << std::format(L"Create dump directory: {}, ec.Value:{}\n", path, ec.value());
        }

        // 덤프 파일 경로 지정
        TCHAR dumpPath[MAX_PATH];
        SYSTEMTIME st;
        GetLocalTime(&st);

        _snwprintf_s(dumpPath, MAX_PATH, L"%scrash_%s_%04d%02d%02d_%02d%02d%02d.dmp",
            path, getApplicationName().c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

        // 파일 생성
        HANDLE hDumpFile = CreateFile(dumpPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hDumpFile != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
            dumpInfo.ThreadId = GetCurrentThreadId();
            dumpInfo.ExceptionPointers = pExceptionInfo;
            dumpInfo.ClientPointers = FALSE;

            // 덤프 파일 작성
            MiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hDumpFile,
                (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory | MiniDumpWithHandleData),
                &dumpInfo,
                nullptr,
                nullptr
            );

            CloseHandle(hDumpFile);
        }
    }

    void WindowDumpMaker::doUnRegist()
    {
        if (_oldFilter) {
            ::SetUnhandledExceptionFilter(_oldFilter);
            _oldFilter = nullptr;
        }
    }

    DumpMaker& GetDumpMaker()
    {
        return WindowDumpMaker::GetInstance();
    }
}

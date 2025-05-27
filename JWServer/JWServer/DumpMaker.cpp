#include "DumpMaker.h"
#include "TypeDefinition.h"
#include <DbgHelp.h>
#include <filesystem>

#pragma comment(lib, "Dbghelp.lib")

namespace jw
{
    void DumpMaker::SetApplicationName(const std::wstring& applicationName)
    {
        _applicationName = applicationName;
    }

    void DumpMaker::Regist()
    {
        auto unhandledExceptionFilterFunc = [](EXCEPTION_POINTERS* pExceptionInfo)->LONG {
            DumpMaker::GetInstance().UnhandledExceptionFilter(pExceptionInfo);
            return EXCEPTION_EXECUTE_HANDLER; // 덤프 파일 경로 지정
            };

        _oldFilter = ::SetUnhandledExceptionFilter(unhandledExceptionFilterFunc);
    }


    void DumpMaker::UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo)
    {
        constexpr const wchar_t* path = L"log\\dump\\";

        if (!std::filesystem::create_directory(path))
        {
            ASSERT_WITH_MSG(false, std::format(L"폴더 생성 실패 : {}", path));
        }

        // 덤프 파일 경로 지정
        TCHAR dumpPath[MAX_PATH];
        SYSTEMTIME st;
        GetLocalTime(&st);

        _snwprintf_s(dumpPath, MAX_PATH, L"%scrash_%s_%04d%02d%02d_%02d%02d%02d.dmp",
            path, _applicationName.c_str(), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

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

    void DumpMaker::UnRegist()
    {
        if (_oldFilter) {
            ::SetUnhandledExceptionFilter(_oldFilter);
            _oldFilter = nullptr;
        }
    }

    DumpMaker& GetDumpMaker()
    {
        return DumpMaker::GetInstance();
    }
}

#pragma once
#ifndef __JW_DUMP_MAKER_H__
#define __JW_DUMP_MAKER_H__
#include <string>
#include <windows.h>
#include "Singleton.hpp"

namespace jw
{
    class DumpMaker
    {
    public:
        DumpMaker() = default;


        void Regist();
        void UnRegist();
    private:
        virtual void doRegist() = 0;
        virtual void doUnRegist() = 0;
    };

    // WindowDumpMaker는 윈도우에서 예외가 발생했을 때 덤프 파일을 생성하는 역할을 합니다.
    class WindowDumpMaker : public Singleton<WindowDumpMaker, DumpMaker>, public DumpMaker
    {
    private:
        void doRegist() override;
        void doUnRegist() override;

        static void unhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo);
        static std::wstring getApplicationName();

        friend class Singleton<WindowDumpMaker>;

        std::wstring _applicationName{};
        LPTOP_LEVEL_EXCEPTION_FILTER _oldFilter{ nullptr };
    };



    DumpMaker& GetDumpMaker();
}

#define DUMP_MAKER 

#endif // __JW_DUMP_MAKER_H__


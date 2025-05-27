#pragma once
#ifndef __JW_DUMP_MAKER_H__
#define __JW_DUMP_MAKER_H__
#include <string>
#include <windows.h>
#include "Singleton.hpp"

namespace jw
{
    class DumpMaker : public Singleton<DumpMaker>
    {
    public:
        void SetApplicationName(const std::wstring& applicationName);
        void Regist();
        void UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo);
        void UnRegist();
    private:
        friend class Singleton<DumpMaker>;

        std::wstring _applicationName{};
        LPTOP_LEVEL_EXCEPTION_FILTER _oldFilter{ nullptr };
    };



    DumpMaker& GetDumpMaker();
}

#define DUMP_MAKER 

#endif // __JW_DUMP_MAKER_H__


﻿#pragma once
#ifndef __JW_ARGUMENTS_H__
#define __JW_ARGUMENTS_H__
#include <memory>
#include <filesystem>
#include "Singleton.hpp"

namespace jw
{
    class ArgumentsHandler;

    // 프로그램에서 전달한 인수를 파싱하고 처리합니다. 
    // 인수 처리 로직은 ArgumentsHandler 클래스가 담당합니다. 
    class Arguments : public Singleton<Arguments>
    {
    public:
        using ArgumentType = char;

        Arguments(const Arguments&) = delete;
        Arguments& operator=(const Arguments&) = delete;

        void Initialize(int argc, ArgumentType* argv[]);
        void HandleArgument();
        void SetHandler(ArgumentsHandler* handler);
        void SetPath(std::filesystem::path path);
        std::filesystem::path GetPath() const;
        std::wstring getProcessName() const;
    protected:
        Arguments();
        ~Arguments();
    private:
        void setAgumentContainer(int argc, ArgumentType* argv[]);
        void makeProcessName(const ArgumentType* arg);

        struct Impl;
        std::unique_ptr<Impl> _pImpl;

        friend class Singleton<Arguments>;
    };

    Arguments& GetArguments();
}

#endif // __JW_ARGUMENTS_H__


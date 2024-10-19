#pragma once
#ifndef __ARGUMENTS_H_
#define __ARGUMENTS_H_
#include <memory>
#include "Singleton.hpp"

namespace jw
{
    class ArgumentsHandler;

    // ���α׷����� ������ �μ��� �Ľ��ϰ� ó���մϴ�. 
    // �μ� ó�� ������ ArgumentsHandler Ŭ������ ����մϴ�. 
    class Arguments : public Singleton<Arguments>
    {
    public:
        Arguments(const Arguments&) = delete;
        Arguments& operator=(const Arguments&) = delete;

        void Initialize(int argc, char* argv[]);
        void HandleArgument();
        void SetHandler(ArgumentsHandler* handler);
    protected:
        Arguments();
        ~Arguments();
    private:
        void setAgumentContainer(int argc, char* argv[]);
        void makeProcessName(const char* arg);

        struct Impl;
        std::unique_ptr<Impl> _pImpl;

        friend class Singleton<Arguments>;
    };

#define ARGUMENT Arguments::GetInstance
}



#endif
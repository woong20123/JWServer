#pragma once
#ifndef __ARGUMENT_HANDLER_H_
#define __ARGUMENT_HANDLER_H_

#include <functional>
#include <string>
#include <vector>

namespace jw
{
    // ���α׷��� ���޹��� �μ����� ó���մϴ�. 
    // ��� �� : "--help" ��� �μ��� ���� ������ �μ� ������ Console ����մϴ�. 
    // ArgumentsHandler�� ��� �޾� registerHandler�� �Լ��� ���� �� �� �ֽ��ϴ�.
    // DefaultArgumentHandler�� ���������� ���� ��Ź �����.
    class ArgumentsHandler
    {
    public:
        using ArgumentContainer = std::vector<std::string >;
        using HandlerKey = std::string;
        using HandlerValue = std::function<void(uint16_t)>;

        ArgumentsHandler();
        virtual ~ArgumentsHandler();
        void Initialize(const std::string& processName);

        void HandleArgument(const ArgumentContainer& arguments);

        const std::string& GetProcessName() const;
    protected:
        bool addHandler(const HandlerKey& key, const HandlerValue& value);
        virtual void registerHandler() = 0;
    private:
        struct Impl;
        std::unique_ptr<Impl> _pImpl;

    };

    // ArgumentsHandler�� ��ӹ޾� �⺻���� ���� ������ �߰��� Ŭ�����Դϴ�. 
    // ���������� �����Ͽ� ����Ŀ� ���缭 �籸�� ���ּ���. 
    class DefaultArgumentHandler : public ArgumentsHandler
    {
    public:
        static constexpr const char* HELP_OPTION = "--help";

        virtual ~DefaultArgumentHandler();
    protected:
        virtual void printHelp();
        void registerHandler() override;
    };
}
#endif

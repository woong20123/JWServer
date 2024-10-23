#pragma once
#ifndef __ARGUMENT_HANDLER_H_
#define __ARGUMENT_HANDLER_H_

#include <functional>
#include <string>
#include <vector>

namespace jw
{
    // 프로그램에 전달받은 인수들을 처리합니다. 
    // 사용 예 : "--help" 라는 인수를 전달 받으면 인수 사용법을 Console 출력합니다. 
    // ArgumentsHandler를 상속 받아 registerHandler를 함수를 구현 할 수 있습니다.
    // DefaultArgumentHandler의 구현사항을 참조 부탁 드려요.
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

    // ArgumentsHandler를 상속받아 기본적인 구현 사항을 추가한 클래스입니다. 
    // 구현사항을 참고하여 사용방식에 맞춰서 재구현 해주세요. 
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

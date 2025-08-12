#pragma once
#ifndef __JW_CONSUMER_HPP__
#define __JW_CONSUMER_HPP__
#include <memory>
#include <thread>
#include <iostream>
#include <list>
#include <string_view>
#include <functional>
#include <cassert>
#include "ProducerContainer.hpp"
#include "StringConverter.h"
#include "ThreadManager.h"
#include "ThreadHelper.h"
namespace jw
{
    // ProducerContainer를 사용하는 소비자 스레드 객체에 대한 일반화된 클래스 규격을 제공합니다. 
    // [object의 이동 순서]
    // 생산자 스레드에서 ProducerContainer::Push 호출
    // 소비자 스레드에서 Consumer::execute -> ProducerContainer::Wait -> Consumer::handle 순서로 호출
    //
    // Consumer를 상속하는 객체는 Consumer::handle(필수), Consumer::prepare 함수를 override 합니다. 
    template<typename object>
    class Consumer
    {
    public:
        using PCContainer = ProducerContainer<object>;
        using container = ProducerContainer<object>::container;
        using HandleFunc = std::function<void(const container& objs)>;
        static constexpr size_t DEFAULT_THREAD_COUNT = 1;

        explicit Consumer();
        explicit Consumer(const std::shared_ptr<PCContainer>& producer, size_t threadCount = DEFAULT_THREAD_COUNT);
        virtual ~Consumer();

        void SetProducerCon(const std::shared_ptr<PCContainer>& producerCon) {
            _pProducerCon = producerCon;
        }

        void SetProducerCon(std::shared_ptr<PCContainer>&& producerCon) {
            _pProducerCon = std::move(producerCon);
        }

        void SetName(const std::string_view name)
        {
            _name = name;
        }

        void SetPrepareFunc(const std::function<void()>& prepareFunc)
        {
            _prepareFunc = prepareFunc;
        }

        void SetHandleFunc(const HandleFunc& handleFunc)
        {
            _handleFunc = handleFunc;
        }

        void RunThread();

        void Stop();

    protected:
        // Producer에서 object를 전달 받는 로직을 실행 전에 수행해야 할 작업을 등록합니다. 
        virtual void prepare();
        // Producer에서 전달 된 object를 handle로 전달합니다. 
        void execute(std::stop_token stoken, std::function<void()> updateExecuteFunc);
        //// 전달 받은 object를 처리하는 로직을 등록합니다. 
        //virtual void handle(const container& objs) = 0;


    private:

        std::string                         _name;
        std::function<void()>               _prepareFunc;
        HandleFunc                          _handleFunc;
        std::shared_ptr<PCContainer>	    _pProducerCon;
        std::vector<std::thread::id>	    _threadIds;
        size_t                              _threadCount;
    };

    template<typename object>
    Consumer<object>::Consumer() : _pProducerCon{ nullptr }, _threadCount{ DEFAULT_THREAD_COUNT },
        _prepareFunc{ nullptr }, _handleFunc{ nullptr }
    {
        static_assert(std::is_pointer_v<object> == false, "Consumer's <object> must not be pointer type ");
        static_assert(std::is_reference_v<object> == false, "Consumer's <object> must not be reference type ");
    }

    template<typename object>
    Consumer<object>::Consumer(const std::shared_ptr<PCContainer>& producer, size_t threadCount) : _pProducerCon{ producer }, _threadCount{ threadCount },
        _prepareFunc{ nullptr }, _handleFunc{ nullptr }
    {
    }

    template<typename object>
    Consumer<object>::~Consumer()
    {
    }

    template<typename object>
    void Consumer<object>::prepare()
    {
    }

    template<typename object>
    void Consumer<object>::RunThread()
    {
        prepare();

        if (_prepareFunc)
            _prepareFunc();

        using namespace std::placeholders;
        _threadIds.reserve(_threadCount);

        for (int i = 0; i < _threadCount; i++)
        {
            const auto threadName = std::format(L"Consumer-{}-{}", StringConverter::StrA2WUseUTF8(_name).value(), i);
            auto t = std::make_unique<Thread>();
            t->Initialize(threadName.data());
            std::thread::id tid = t->GetThraedId();
            t->SetExecution(std::bind(&Consumer<object>::execute, this, _1, _2), [tPtr = t.get()]() { tPtr->UpdateLastExecutionTime(); });
            _threadIds.emplace_back(tid);
            GetThreadManager().AddThread(std::move(t));
        }
    }

    template<typename object>
    void Consumer<object>::Stop()
    {
        for (const auto threadId : _threadIds)
        {
            GetThreadManager().StopThread(threadId);
        }
    }

    template<typename object>
    void Consumer<object>::execute(std::stop_token stoken, std::function<void()> updateExecuteFunc)
    {
        assert(_handleFunc);
        while (true)
        {

            if (stoken.stop_requested())
            {
                // 이전에 등록된 모든 queueObject가 다 처리 되었다면 종료, 최대 대기 시간도 필요 할 듯
                if (0 == _pProducerCon->Size())
                {
                    std::cerr << std::format("Producer<{}> {}  is stop\n", typeid(object).name(), _name);
                    break;
                }
                else
                {
                    std::cerr << std::format("Producer<{}> {} remain _pProducerCon->Size() = {} \n", typeid(object).name(), _name, _pProducerCon->Size());
                }
            }
            updateExecuteFunc();

            container queueObjects;
            _pProducerCon->Wait(queueObjects);

            if (!queueObjects.empty())
                _handleFunc(queueObjects);
        }
    }

}


#define CONSUMER_BASE_DECLARE(CLASSNAME, OBJ) \
public: \
    using PCContainer = ProducerContainer<std::shared_ptr<OBJ>>; \
    CLASSNAME() : Consumer() {} \
    virtual ~CLASSNAME() = default; \
    CLASSNAME(const std::shared_ptr<PCContainer>& producer) : Consumer(producer) {} \
    CLASSNAME(const std::shared_ptr<PCContainer>& producer, const size_t threadCount) : Consumer(producer, threadCount) {} \
private: \
    void handle(const container & objs) override;

#endif //__JW_CONSUMER_HPP__
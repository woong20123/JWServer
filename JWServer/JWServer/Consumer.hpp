#pragma once
#ifndef __JW_CONSUMER_HPP__
#define __JW_CONSUMER_HPP__
#include <memory>
#include <thread>
#include <iostream>
#include <list>
#include "ProducerContainer.hpp"
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
        using obj = object;
        using PCContainer = ProducerContainer<object>;
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

        void SetName(const std::string& name)
        {
            _name = name;
        }

        void SetName(std::string&& name)
        {
            _name = std::move(name);
        }

        void RunThread();

    protected:
        // Producer에서 object를 전달 받는 로직을 실행 전에 수행해야 할 작업을 등록합니다. 
        virtual void prepare();
        // Producer에서 전달 된 object를 handle로 전달합니다. 
        void execute();
        // 전달 받은 object를 처리하는 로직을 등록합니다. 
        virtual void handle(const std::list<obj>& objs) = 0;

        void joinWaitThread();

    private:
        inline bool isStop() { return 0 != _stopTimeTick; }

        std::string                         _name;
        std::shared_ptr<PCContainer>	    _pProducerCon;
        std::vector<std::thread>	        _threads;
        size_t                              _threadCount{ 1 };
        std::atomic<time_t>                 _stopTimeTick{ 0 };
    };

    template<typename object>
    Consumer<object>::Consumer() : _pProducerCon{ nullptr }
    {
        static_assert(std::is_pointer<object>::value == false, "Consumer's <object> must not be pointer type ");
    }

    template<typename object>
    Consumer<object>::Consumer(const std::shared_ptr<PCContainer>& producer, size_t threadCount) : _pProducerCon{ producer }, _threadCount{ threadCount }
    {
    }

    template<typename object>
    Consumer<object>::~Consumer()
    {
        joinWaitThread();
    }

    template<typename object>
    void Consumer<object>::prepare()
    {
    }

    template<typename object>
    void Consumer<object>::RunThread()
    {
        prepare();

        for (int i = 0; i < _threadCount; i++)
            _threads.emplace_back(&Consumer::execute, this);
    }

    template<typename object>
    void Consumer<object>::execute()
    {
        while (true)
        {
            if (isStop())
            {
                // 이전에 등록된 모든 queueObject가 다 처리 되었다면 종료, 최대 대기 시간도 필요 할 듯
                if (0 == _pProducerCon->Size())
                {
                    std::cerr << std::format("Producer<{}> {}  is stop\n", typeid(obj).name(), _name);
                    break;
                }
            }
            else
            {
                if (_pProducerCon->IsStop())
                {
                    _stopTimeTick = ::time(nullptr);
                }
            }

            std::list<obj> queueObjects;
            _pProducerCon->Wait(queueObjects);
            if (!queueObjects.empty())
                handle(queueObjects);
        }
    }

    template<typename object>
    void Consumer<object>::joinWaitThread()
    {
        for (auto& t : _threads)
        {
            if (t.joinable())
                t.join();
        }
    }
}



#define CONSUMER(OBJ) Consumer<OBJ>

#define CONSUMER_BASE_DECLARE(CLASSNAME, OBJ) \
public: \
    using PCContainer = ProducerContainer<std::shared_ptr<OBJ>>; \
    CLASSNAME() : Consumer() {} \
    virtual ~CLASSNAME() {} \
    CLASSNAME(const std::shared_ptr<PCContainer>& producer) : Consumer(producer) {} \
    CLASSNAME(const std::shared_ptr<PCContainer>& producer, const size_t threadCount) : Consumer(producer, threadCount) {} \
private: \
    void handle(const std::list<obj>& objs) override;

#endif //__JW_CONSUMER_HPP__
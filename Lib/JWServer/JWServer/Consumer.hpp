#pragma once
#ifndef __CONSUMER_HPP_
#define __CONSUMER_HPP_
#include <memory>
#include <thread>
#include <iostream>
#include "Producer.hpp"
namespace jw
{
    /// <summary>
    /// 등록된 Producer의 Queue를 실시간으로 감지하여 데이터가 들어오면
    /// Handle함수로 보내집니다.
    /// Handle함수는 사용자가 정의합니다.
    /// </summary>
    /// <typeparam name="Producer"></typeparam>
    template<typename object>
    class Consumer
    {
    public:
        using queueObject = object;
        using ProducerObj = Producer<object>;
        static constexpr size_t DEFAULT_THREAD_COUNT = 1;

        explicit Consumer();
        explicit Consumer(const std::shared_ptr<ProducerObj>& producer, size_t threadCount = DEFAULT_THREAD_COUNT);
        virtual ~Consumer();

        void SetProducer(std::shared_ptr<ProducerObj>& producer) {
            _pProducer = producer;
        }

        void SetName(const std::string& name)
        {
            _name = name;
        }

        void RunThread();

    protected:
        virtual void prepare();										///  프로세서를 실행하기 전에 해야 할 작업을 등록합니다.
        void execute();										        ///  생산자 큐에서 오브젝트를 받아와 Handler에 넘깁니다.
        virtual void handle(const queueObject& obj) = 0;			///  생산자의 object를 처리합니다.

    private:
        std::string                         _name;
        std::shared_ptr<ProducerObj>	    _pProducer;
        std::vector<std::thread>	        _threads;
        size_t                              _threadCount{ 1 };
    };

    template<typename object>
    Consumer<object>::Consumer() : _pProducer{ nullptr }
    {}

    template<typename object>
    Consumer<object>::Consumer(const std::shared_ptr<ProducerObj>& producer, size_t threadCount) : _pProducer{ producer }, _threadCount{ threadCount }
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
        for (int i = 0; i < _threadCount; i++)
            _threads.push_back(std::thread(&Consumer::execute, this));
    }

    template<typename object>
    void Consumer<object>::execute()
    {
        prepare();

        while (true)
        {
            if (_pProducer->IsStop())
            {
                std::cerr << "Producer<" << typeid(queueObject).name() << "> " << _name.c_str() << " is stop" << std::endl;
                break;
            }

            std::list<queueObject> queueObjects;
            _pProducer->Wait(queueObjects);
            for (const auto& queueObject : queueObjects)
                handle(queueObject);
        }
    }

}

#define CONSUMER(OBJ) Consumer<OBJ>

#define CONSUMER_BASE_DECLARE(CLASSNAME, OBJ) \
public: \
    using ProducerObj = Producer<std::shared_ptr<OBJ>>; \
    CLASSNAME() : Consumer() {} \
    virtual ~CLASSNAME() {} \
    CLASSNAME(const std::shared_ptr<ProducerObj>& producer) : Consumer(producer) {} \
    CLASSNAME(const std::shared_ptr<ProducerObj>& producer, const size_t threadCount) : Consumer(producer, threadCount) {} \
private: \
    void handle(const Consumer::queueObject& obj) override;

#endif
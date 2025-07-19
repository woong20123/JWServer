#pragma once
#ifndef __JW_PRODUCER_HPP__
#define __JW_PRODUCER_HPP__
#include <cstdint>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <chrono>
#include <list>
namespace jw
{
    // 생산자 스레드와 소비자 스레드간에 object를 전달하기 위한 container 객체입니다. 
    // [object의 이동 순서]
    // 생산자 스레드에서 ProducerConsumerContainer::Push 호출
    // 소비자 스레드에서 Consumer::execute -> ProducerConsumerContainer::Wait -> Consumer::handle 순서로 호출
    template<typename object>
    class ProducerContainer
    {
    public:
        using container = std::list<object>;

        ProducerContainer(uint32_t durationMSecond);
        virtual ~ProducerContainer();

        void							Wait(container& objList);
        void                            Wait(container& objList, uint32_t durationMilliseconds);
        void							Push(const object& obj);
        void							Flush();
        size_t							Size();
    private:
        std::condition_variable_any			_cv;
        std::shared_mutex					_shared_mutex;
        container	            			_list;
        uint32_t                            _waitdurationMsecs;
    };

    template<typename object>
    ProducerContainer<object>::ProducerContainer(uint32_t durationMsecs) :
        _waitdurationMsecs{ durationMsecs }
    {}
    template<typename object>
    ProducerContainer<object>::~ProducerContainer()
    {}

    template<typename object>
    void ProducerContainer<object>::Wait(container& objList)
    {
        Wait(objList, _waitdurationMsecs);
    }

    template<typename object>
    void ProducerContainer<object>::Wait(container& objList, uint32_t durationMilliseconds)
    {
        using namespace std::chrono_literals;
        std::unique_lock<std::shared_mutex> lk(_shared_mutex);
        std::chrono::duration < int64_t, std::milli> duration{ durationMilliseconds };
        _cv.wait_for(lk, duration, [this] {return !_list.empty(); });

        if (_list.empty()) return;

        objList.splice(objList.begin(), _list);
    }

    template<typename object>
    void ProducerContainer<object>::Push(const object& obj)
    {
        std::unique_lock<std::shared_mutex> lk(_shared_mutex);
        _list.push_back(obj);
    }

    template<typename object>
    void ProducerContainer<object>::Flush()
    {
        _cv.notify_one();
    }

    template<typename object>
    size_t ProducerContainer<object>::Size()
    {
        std::shared_lock<std::shared_mutex> lk(_shared_mutex);
        return _list.size();
    }
}

#define PCCON(obj)  jw::ProducerContainer<obj>
#endif // __JW_PRODUCER_HPP__
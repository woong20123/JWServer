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
    /// <summary>
    /// Producer는 Queue 보유합니다.
    /// Queue는 템플릿타입의 정보를 저장하고 있습니다.
    /// Producer를 Consumer에 등록하면 Consumer에서는 Producer의 Queue를 감시하여 데이터가 들어오면 처리합니다.
    /// </summary>
    /// <typeparam name="object"></typeparam>
    template<typename object>
    class Producer
    {
    public:
        using queueObject = object;

        Producer();
        virtual ~Producer();

        void							Wait(std::list<object>& objList);
        void							Push(const object& obj);
        void							Flush();
        void							SetStopSignal();
        bool							IsStop();
        size_t							Size();
    private:
        std::condition_variable_any			_cv;
        std::shared_mutex					_shared_mutex;
        std::list<queueObject>				_list;
        size_t								_popMaxNumberPerTick;
        std::atomic<bool>					_isStop;
    };

    template<typename object>
    Producer<object>::Producer() : _isStop{ false }
    {}
    template<typename object>
    Producer<object>::~Producer()
    {}

    template<typename object>
    void Producer<object>::Wait(std::list<object>& objList)
    {
        using namespace std::chrono_literals;
        std::unique_lock<std::shared_mutex> lk(_shared_mutex);
        _cv.wait_for(lk, 100ms, [this] {return !_list.empty(); });

        if (_list.empty()) return;

        objList.splice(objList.begin(), _list);
    }

    template<typename object>
    void Producer<object>::Push(const object& obj)
    {
        std::unique_lock<std::shared_mutex> lk(_shared_mutex);
        _list.push_back(obj);
    }

    template<typename object>
    void Producer<object>::Flush()
    {
        _cv.notify_one();
    }
    template<typename object>
    void Producer<object>::SetStopSignal()
    {
        _isStop.store(true);
    }

    template<typename object>
    bool Producer<object>::IsStop()
    {
        return _isStop.load();
    }

    template<typename object>
    size_t Producer<object>::Size()
    {
        std::shared_lock<std::shared_mutex> lk(_shared_mutex);
        return _list.size();
    }
}
#endif // __JW_PRODUCER_HPP__
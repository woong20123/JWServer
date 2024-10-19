#pragma once
#ifndef __PRODUCER_HPP_
#define __PRODUCER_HPP_
#include <cstdint>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <chrono>
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
		static constexpr size_t DEFAULT_POP_MAX_NUMBER_PER_TICK = 100;
		using QueueObj = std::shared_ptr<object>;

		Producer();
		Producer(size_t popMaxNumberPerTick);
		virtual ~Producer();

		void							Pop(std::vector<QueueObj>& objList);
		void							Push(const QueueObj & obj);
		void							Flush();
		void							SetStopSignal();
		bool							IsStop();
		size_t							Size();
	private:
		std::condition_variable_any			_cv;
		std::shared_mutex					_shared_mutex;
		std::queue<QueueObj>				_queue;
		size_t								_popMaxNumberPerTick;
		std::atomic<bool>					_isStop;
	};

	template<typename object>
	Producer<object>::Producer() : Producer(DEFAULT_POP_MAX_NUMBER_PER_TICK)
	{}

	template<typename object>
	Producer<object>::Producer(size_t popMaxNumberPerTick) : _popMaxNumberPerTick{ popMaxNumberPerTick }, _isStop{false}
	{}
	template<typename object>
	Producer<object>::~Producer()
	{}

	template<typename object>
	void Producer<object>::Pop(std::vector<QueueObj> & objList)
	{
		using namespace std::chrono_literals;
		std::unique_lock<std::shared_mutex> lk(_shared_mutex);
		_cv.wait_for(lk, 100ms, [this] {return !_queue.empty(); });

		for (int i = 0; i < _popMaxNumberPerTick && !_queue.empty() ; ++i)
		{
			QueueObj obj = _queue.front();
			_queue.pop();
		}
	}

	template<typename object>
	void Producer<object>::Push(const QueueObj & obj)
	{
		std::unique_lock<std::shared_mutex> lk(_shared_mutex);
		_queue.push(obj);
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
		return _queue.size();
	}
}
#endif

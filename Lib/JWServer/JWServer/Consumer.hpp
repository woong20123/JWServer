#pragma once
#ifndef __CONSUMER_HPP_
#define __CONSUMER_HPP_
#include <memory>
#include <thread>
namespace jw
{
	/// <summary>
	/// 등록된 Producer의 Queue를 실시간으로 감지하여 데이터가 들어오면
	/// Handle함수로 보내집니다.
	/// Handle함수는 사용자가 정의합니다.
	/// </summary>
	/// <typeparam name="Producer"></typeparam>
	template<typename Producer>
	class Consumer
	{
	public:
		using queueObject = Producer::queueObj;

		explicit Consumer(std::shared_ptr<Producer>& producer, size_t thread_count);
		virtual ~Consumer();

	protected:
		virtual void Prepare();										///  프로세서를 실행하기 전에 해야 할 작업을 등록합니다.
		virtual void Execute();										///  생산자 큐에서 오브젝트를 받아와 Handler에 넘깁니다.
		virtual void Handle(const queueObject& obj) = 0;			///  생산자의 object를 처리합니다.
		virtual void ThreadRun();

	private:
		void Run();
		std::shared_ptr<Producer>	_pProducer;
		std::thread					_thread;
	};


	template<typename Producer>
	Consumer<Producer>::Consumer(std::shared_ptr<Producer> & producer, size_t thread_count)
	{
		mImpl = new Impl;
		_pProducer.reset(producer);
	}

	template<typename Producer>
	Consumer<Producer>::~Consumer()
	{
	}

	template<typename Producer>
	void Consumer<Producer>::Prepare()
	{
	}

	template<typename Producer>
	void Consumer<Producer>::ThreadRun()
	{
		for (int i = 0; i < mImpl->nRuncount_; i++) Run();
	}

	template<typename Producer>
	void Consumer<Producer>::Execute()
	{
		Prepare();

		while (true) 
		{
			if (_pProducer->IsStop())
			{
				std::cout << "Producer<" << typeid(queueObject).name() << "> is stop" << std::endl;
				break;
			}

			std::vector<queueObject> queueObjects;
			_pProducer->pop(queueObjects);
			for (const auto& queueObject : queueObjects)
				Handle(queueObject);

		}
	}

	template<typename Producer>
	void Consumer<Producer>::Run()
	{
		_thread = std::thread(&Consumer::Execute, this);
	}
}
#endif

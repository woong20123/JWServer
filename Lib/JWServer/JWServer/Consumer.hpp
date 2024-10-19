#pragma once
#ifndef __CONSUMER_HPP_
#define __CONSUMER_HPP_
#include <memory>
#include <thread>
namespace jw
{
	/// <summary>
	/// ��ϵ� Producer�� Queue�� �ǽð����� �����Ͽ� �����Ͱ� ������
	/// Handle�Լ��� �������ϴ�.
	/// Handle�Լ��� ����ڰ� �����մϴ�.
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
		virtual void Prepare();										///  ���μ����� �����ϱ� ���� �ؾ� �� �۾��� ����մϴ�.
		virtual void Execute();										///  ������ ť���� ������Ʈ�� �޾ƿ� Handler�� �ѱ�ϴ�.
		virtual void Handle(const queueObject& obj) = 0;			///  �������� object�� ó���մϴ�.
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

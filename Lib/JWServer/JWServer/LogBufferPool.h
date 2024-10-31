#pragma once
#ifndef __JW_LOG_BUFFER_POOL_H__
#define __JW_LOG_BUFFER_POOL_H__
#include <memory>
#include "Singleton.hpp"

namespace jw
{
	class LogBuffer;
	class LogBufferPool : public Singleton<LogBufferPool>
	{
	public:

		LogBuffer * Acquire();
		void Release(LogBuffer *& obj);
	protected:
		LogBufferPool();
		~LogBufferPool();
	private:
		struct Impl;
		std::unique_ptr<Impl> _mImpl;
		friend class Singleton<LogBufferPool>;
	};
}

#define LOG_BUFFER_POOL jw::LogBufferPool::GetInstance
#endif // !__JW_LOG_BUFFER_POOL_H__

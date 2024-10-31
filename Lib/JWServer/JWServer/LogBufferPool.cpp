#include "LogBufferPool.h"
#include "LogBuffer.h"
#include <iostream>

namespace jw
{
	struct LogBufferPool::Impl
	{
		std::atomic<uint64_t> useCount;
	};

	LogBufferPool::LogBufferPool() : _mImpl{std::make_unique<Impl>()}
	{}

	LogBufferPool::~LogBufferPool()
	{}

	LogBuffer * LogBufferPool::Acquire()
	{
		++_mImpl->useCount;
		return new LogBuffer;
	}

	void LogBufferPool::Release(LogBuffer *& obj)
	{
		delete obj;
		obj = nullptr;
		--_mImpl->useCount;
	}
}
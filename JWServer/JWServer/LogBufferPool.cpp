#include "LogBufferPool.h"
#include "LogBuffer.h"

namespace jw
{
	LogBufferPool::LogBufferPool()
	{}

	LogBuffer* LogBufferPool::Acquire()
	{

		LogBuffer* ret{ _objectPool.Acquire() };
		++_useCount;
		return ret;
	}

	void LogBufferPool::Release(LogBuffer* obj)
	{
		if (!obj) return;
		_objectPool.Release(obj);
		--_useCount;
	}

	uint64_t LogBufferPool::GetUseCount()
	{
		uint64_t useCount = _useCount;
		return useCount;
	}

}
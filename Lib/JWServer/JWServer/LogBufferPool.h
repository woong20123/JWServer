#pragma once
#ifndef __JW_LOG_BUFFER_POOL_H__
#define __JW_LOG_BUFFER_POOL_H__
#include <memory>
#include "Singleton.hpp"
#include "ObjectPool.hpp"

namespace jw
{
	class LogBuffer;
	class LogBufferPool : public Singleton<LogBufferPool>
	{
	public:
		static constexpr size_t BASE_ALLOCATE_COUNT = 16;
		static constexpr size_t MAX_ALLOCATE_COUNT = 512;

		LogBuffer* Acquire();
		void Release(LogBuffer* obj);
	protected:
		LogBufferPool();
	private:
		std::atomic<uint64_t>											_useCount;
		ObjectPool<LogBuffer, BASE_ALLOCATE_COUNT, MAX_ALLOCATE_COUNT>	_objectPool;
		friend class Singleton<LogBufferPool>;
	};
}

#define LOG_BUFFER_POOL jw::LogBufferPool::GetInstance
#endif // !__JW_LOG_BUFFER_POOL_H__

#pragma once
#ifndef __JW_OBJECT_POOL_HPP__
#define __JW_OBJECT_POOL_HPP__
#include <list>
#include <atomic>
#include <shared_mutex>
#include "TypeDefinition.h"

namespace jw
{
	template<typename T, size_t BASE_ALLOCATE_COUNT, size_t MAX_ALLOCATE_COUNT>
	class ObjectPool
	{
	public:
		ObjectPool() {}

		T* Acquire()
		{
			T* ret{ nullptr };
			{
				std::unique_lock<std::shared_mutex> lk{ _listMutex };

				if (_list.size() == 0)
					allocate();

				ret = _list.front();
				_list.pop_front();
			}
			return ret;
		}
		void Release(T* object)
		{
			{
				std::unique_lock<std::shared_mutex> lk{ _listMutex };
				_list.push_back(object);
			}
		}
	private:

		void allocate()
		{
			T* ptr = reinterpret_cast<T*>(::operator new(_allocateCount * sizeof(T)));
			for (int i = 0; i < _allocateCount; ++i)
			{
				_list.push_back(new(ptr + i) T);
			}

			// allicateCount를 2배씩 증가시켜 최대 MAX_ALLOCATE_COUNT까지 증가 시킵니다.
			if (_allocateCount < MAX_ALLOCATE_COUNT)
			{
				_allocateCount = JW_MIN(_allocateCount * 2, MAX_ALLOCATE_COUNT);
			}
		}

		std::list<T*>			_list;
		std::shared_mutex		_listMutex;
		size_t					_allocateCount{ BASE_ALLOCATE_COUNT };
	};
}

#endif // !__JW_OBJECT_POOL_HPP__
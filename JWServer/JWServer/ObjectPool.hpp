#pragma once
#ifndef __JW_OBJECT_POOL_HPP__
#define __JW_OBJECT_POOL_HPP__
#include <list>
#include <atomic>
#include <shared_mutex>
#include "TypeDefinition.h"

namespace jw
{
	template<typename T, size_t BASE_ALLOCATE_COUNT = 16, size_t MAX_ALLOCATE_COUNT = 32768>
	class ObjectPool
	{
	public:
		using Type = std::decay_t<std::remove_pointer_t<T>>;
		using Pointer = Type*;
		using List = std::list<Pointer>;

		ObjectPool()
		{
		}

		Pointer Acquire()
		{
			Pointer ret{ nullptr };
			{
				std::unique_lock<std::shared_mutex> lk{ _listMutex };

				if (_list.size() == 0)
					allocate();

				ret = _list.front();
				_list.pop_front();
			}
			return ret;
		}
		void Release(Pointer object)
		{
			std::unique_lock<std::shared_mutex> lk{ _listMutex };
			_list.push_back(object);
		}
	private:

		void allocate()
		{
			Pointer ptr = reinterpret_cast<Pointer>(::operator new(_allocateCount * sizeof(Type)));
			for (int i = 0; i < _allocateCount; ++i)
			{
				_list.push_back(new(ptr + i) Type);
			}

			// allicateCount를 2배씩 증가시켜 최대 MAX_ALLOCATE_COUNT까지 증가 시킵니다.
			if (_allocateCount < MAX_ALLOCATE_COUNT)
			{
				_allocateCount = JW_MIN(_allocateCount * 2, MAX_ALLOCATE_COUNT);
			}
		}

		List					_list;
		std::shared_mutex		_listMutex;
		size_t					_allocateCount{ BASE_ALLOCATE_COUNT };
	};
}

#endif // !__JW_OBJECT_POOL_HPP__
#pragma once
#ifndef __JW_OBJECT_POOL_HPP__
#define __JW_OBJECT_POOL_HPP__
#include <list>
#include <atomic>
#include <shared_mutex>
#include "TypeDefinition.h"

namespace jw
{
	template<typename T>
	concept IsObjectPoolable = !std::is_pointer_v<T> && !std::is_array_v <T> && !std::is_reference_v <T>;


	template<typename T, size_t BASE_ALLOCATE_COUNT = 16, size_t MAX_ALLOCATE_COUNT = 32768>
		requires IsObjectPoolable<T>
	class ObjectPool
	{
	public:
		using ValueType = T;
		using Reference = ValueType&;
		using Pointer = ValueType*;
		using List = std::list<Pointer>;

		ObjectPool()
		{
			// T를 전방선언으로 사용할 경우 T에 대한 구현 내용을 알 수 없으므로 생성자에서 기본 생성자 여부를 확인합니다. 
			static_assert(std::is_default_constructible_v<T>, "not supported default construct");
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
			Pointer ptr = reinterpret_cast<Pointer>(::operator new(_allocateCount * sizeof(ValueType)));
			for (int i = 0; i < _allocateCount; ++i)
			{
				_list.push_back(new(ptr + i) ValueType);
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
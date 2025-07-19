#pragma once
#ifndef __JW_SINGLETON_HPP__
#define __JW_SINGLETON_HPP__
#include <concepts>

namespace jw
{
    template<typename PIMPL, typename INTERFACE = PIMPL>
    class Singleton
    {
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;
        Singleton& operator=(Singleton&&) = delete;

        static INTERFACE& GetInstance()
            requires std::derived_from<PIMPL, INTERFACE>
        {
            static PIMPL intance;
            return intance;
        }
    protected:
        Singleton() = default;
        ~Singleton() = default;
    private:
    };
}

#endif // !__JW_SINGLETON_HPP__
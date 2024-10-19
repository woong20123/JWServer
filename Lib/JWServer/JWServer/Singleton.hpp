#pragma once
#ifndef __SINGLETON_HPP_
#define __SINGLETON_HPP_

namespace jw
{
    template<typename T>
    class Singleton
    {
    public:
        static T& GetInstance()
        {
            static T intance;
            return intance;
        }
    protected:
        Singleton() {}
        ~Singleton() {}
    private:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    };
}

#endif // __SINGLETON_HPP_
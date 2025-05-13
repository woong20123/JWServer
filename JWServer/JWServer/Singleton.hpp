#pragma once
#ifndef __JW_SINGLETON_HPP__
#define __JW_SINGLETON_HPP__

namespace jw
{
    template<typename T>
    class Singleton
    {
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        static T& GetInstance()
        {
            static T intance;
            return intance;
        }
    protected:
        Singleton() {}
        ~Singleton() {}
    private:
    };
}

#endif // !__JW_SINGLETON_HPP__
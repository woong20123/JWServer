#pragma once
#ifndef __JW_RANDOM_H__
#define __JW_RANDOM_H__
#include <random>
#include <cassert>

namespace jw
{
    class IntRandomGenerator
    {
    public:
        IntRandomGenerator(int high, int low = 0) : _distribution(low, high) {}
        int operator()() { return _distribution(_engine); }
    private:
        std::uniform_int_distribution<int> _distribution;
        std::mt19937 _engine{ std::random_device{}() };
    };

    /// <summary>
    /// range = [low, high]
    /// </summary>
    /// <typeparam name="high"></typeparam>
    class RandomDice
    {
    public:
        constexpr static int DEFAULT_LOW = 1; // Default low value for dice rolls
        static int Roll(int high, int low = DEFAULT_LOW)
        {
            assert(high > low);
            // 여러 스레드에서 동시에 호출되는 경우 성능 저하가 발생 하므로 스레드 로컬 변수로 생성.
            static thread_local IntRandomGenerator gen{ high, low };
            return gen();
        }
    };

    // [1, max] 범위의 확률을 가지는 랜덤 클래스
    template<int max>
    class RandomProbability
    {
    public:
        static bool IsSuccess(int prob)
        {
            assert(prob <= max);
            return RandomDice::Roll(max) <= prob;
        }

        static int GetRandomValue()
        {
            return RandomDice::Roll(max);
        }
    };
}
#endif // __JW_RANDOM_H__


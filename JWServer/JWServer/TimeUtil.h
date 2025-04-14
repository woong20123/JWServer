#pragma once
#include <cstdint>
#include <chrono>
#ifndef __JW_TIME_UTIL_H__
#define __JW_TIME_UTIL_H__
namespace jw
{
    class TimeUtil
    {
    public:
        // 1초를 밀리세컨드로 변환
        constexpr static int64_t SECOND_TO_MILLISECOND = 1000;
        // 1분을 밀리세컨드로 변환
        constexpr static int64_t MINUTE_TO_MILLISECOND = 60 * SECOND_TO_MILLISECOND;
        // 1시간을 밀리세컨드로 변환
        constexpr static int64_t HOUR_TO_MILLISECOND = 60 * MINUTE_TO_MILLISECOND;
        // 1일을 밀리세컨드로 변환
        constexpr static int64_t DAY_TO_MILLISECOND = 24 * HOUR_TO_MILLISECOND;

        static int64_t GetCurrentTimeMilliSecond()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    };
}

#endif // !__JW_TIME_UTIL_H__


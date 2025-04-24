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
        // 1초를 세컨드로 변환
        constexpr static int64_t SECOND_TO_SECOND = 1;
        // 1분을 세컨드로 변환
        constexpr static int64_t MINUTE_TO_SECOND = 60;
        // 1시간을 세컨드로 변환
        constexpr static int64_t HOUR_TO_SECOND = 60 * MINUTE_TO_SECOND;
        // 1일을 세컨드로 변환
        constexpr static int64_t DAY_TO_SECOND = 24 * HOUR_TO_SECOND;

        // 1초를 밀리세컨드로 변환
        constexpr static int64_t SECOND_TO_MILLISECOND = SECOND_TO_SECOND * 1000;
        // 1분을 밀리세컨드로 변환
        constexpr static int64_t MINUTE_TO_MILLISECOND = MINUTE_TO_SECOND * 1000;
        // 1시간을 밀리세컨드로 변환
        constexpr static int64_t HOUR_TO_MILLISECOND = HOUR_TO_SECOND * 1000;
        // 1일을 밀리세컨드로 변환
        constexpr static int64_t DAY_TO_MILLISECOND = DAY_TO_SECOND * 1000;



        static time_t GetCurrentUTCTimeSecond()
        {
            return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        }

        static int64_t GetCurrentTimeMilliSecond()
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }
    };
}

#endif // !__JW_TIME_UTIL_H__


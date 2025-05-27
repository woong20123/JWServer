#pragma once
#include <cstdint>
#include <chrono>
#include <ctime>
#ifndef __JW_TIME_UTIL_H__
#define __JW_TIME_UTIL_H__
namespace jw
{
    struct TimeInfo
    {
        int16_t year{ 0 };
        int16_t month{ 0 };
        int16_t day{ 0 };
        int16_t hour{ 0 };
        int16_t minute{ 0 };
        int64_t second{ 0 };
    };

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

        static time_t GetCurrentTimeT()
        {
            using namespace std::chrono;
            return system_clock::to_time_t(std::chrono::system_clock::now());
        }

        static int64_t GetCurrentTimeMilliSecond()
        {
            using namespace std::chrono;
            return duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        }

        static TimeInfo GetCurrentTimeInfo()
        {
            time_t now = GetCurrentTimeT();
            tm tmNow;
            localtime_s(&tmNow, &now);
            return ToTimeInfo(&tmNow);
        }

        static TimeInfo ToTimeInfo(tm* ptm)
        {
            TimeInfo timeInfo;
            timeInfo.year = ptm->tm_year + 1900;
            timeInfo.month = ptm->tm_mon + 1;
            timeInfo.day = ptm->tm_mday;
            timeInfo.hour = ptm->tm_hour;
            timeInfo.minute = ptm->tm_min;
            timeInfo.second = ptm->tm_sec;
            return timeInfo;
        }
    };
}

#endif // !__JW_TIME_UTIL_H__


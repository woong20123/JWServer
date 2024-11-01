#pragma once
#ifndef __JW_STOP_WATCH_H__
#define __JW_STOP_WATCH_H__



#include <chrono>
#include <iostream>
#include <cassert>
#include <memory>

namespace jw {

    class TimerPrinter
    {
    public:
        virtual void Print(int64_t duration) = 0;
    };

    class StringTimerPrinter : public TimerPrinter
    {
    public:
        StringTimerPrinter(const char* name) : _name{ name } {}
        void Print(int64_t duration) override
        {
            std::cout << _name.c_str() << ": " << duration << "ms. \n";
        }
    private:
        std::string _name;
    };

    class WStringTimerPrinter : public TimerPrinter
    {
    public:
        WStringTimerPrinter(const wchar_t* name) : _name{ name } {}
        void Print(int64_t duration) override
        {
            std::wcout << _name.c_str() << L": " << duration << L"ms. \n";
        }
    private:
        std::wstring _name;
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    class StopWatchTimer {
    public:
        using ClockType = std::chrono::steady_clock;

        StopWatchTimer() {}
        ~StopWatchTimer() = default;

        void Tic()
        {
            _startTimePoint = ClockType::now();
        }

        int64_t Toc()
        {
            using namespace::std::chrono;
            auto stopTimePoint = ClockType::now();
            auto duration = (stopTimePoint - _startTimePoint);
            return duration_cast<milliseconds>(duration).count();
        }

    private:
        ClockType::time_point _startTimePoint{};
    };

    class ScopeTimer {
    public:
        ScopeTimer(TimerPrinter * printer) : _printer{ printer }
        {
            _stopWatchTimer.Tic();
        }
        ScopeTimer(const ScopeTimer&) = delete;
        ScopeTimer(ScopeTimer&&) = delete;
        auto operator=(const ScopeTimer&)->ScopeTimer & = delete;
        auto operator=(ScopeTimer&&)->ScopeTimer & = delete;

        ~ScopeTimer()
        {
            auto durationMilliSeconds = _stopWatchTimer.Toc();
            if (!_printer.get()) {
                assert(false && "printer is null!");
            }
            else
            {
                _printer->Print(durationMilliSeconds);
            }
        }

    private:
        StopWatchTimer _stopWatchTimer;
        std::shared_ptr<TimerPrinter> _printer;
    };
}

#define SCOPE_TIMER(name)  jw::ScopeTimer st{ new jw::WStringTimerPrinter(name) }

#endif // !__JW_STOP_WATCH_H__

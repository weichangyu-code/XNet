#pragma once
#include "CommonPublic.h"

namespace XNet
{
    class TimerManager
    {
        friend class Timer;
    public:
        TimerManager();
        ~TimerManager();

        void addTimer(unsigned int ms, const std::function<void()>& cb);

        void run();
        void stop();

    protected:
        unsigned int _getNewTimerId();
        void _delTimer(unsigned int timerId);
        void _addTimer(unsigned int ms, const std::function<void()>& cb, unsigned int& timerId);

    protected:
        std::mutex _mtx;
        struct TimerInfo
        {
            std::function<void()> cb;
            std::map<unsigned int, std::multimap<unsigned long long, TimerInfo>::iterator>::iterator it;
        };
        std::multimap<unsigned long long, TimerInfo> _timers;

        unsigned int _timerIdSeek = 0;
        std::map<unsigned int, std::multimap<unsigned long long, TimerInfo>::iterator> _timerIdMap;
    };

    class Timer
    {
    public:
        Timer(TimerManager* manager);

        void start(unsigned int ms, const std::function<void()>& cb);
        void stop();

    protected:
        std::mutex _mtx;
        TimerManager* _manager = nullptr;
        unsigned int _timerId = 0;
    };

} // namespace XNet

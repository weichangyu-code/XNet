#pragma once
#include "CommonPublic.h"

namespace XNet
{
    class TimerManager
    {
    public:
        TimerManager();
        ~TimerManager();

        void addTimer(unsigned int ms, const std::function<void()>& cb);

        void run();
        void stop();

    protected:
        std::mutex _mtx;
        std::multimap<unsigned long long, std::function<void()>> _timers;
    };
} // namespace XNet

#pragma once
#include "CommonPublic.h"

namespace XNet
{
    class WatchTimer
    {
    public:
        WatchTimer();

        unsigned int howLong();
        void call(unsigned int interval, std::function<void()> func);
        bool check(unsigned int interval);

    protected:
        unsigned int _clock;
    };
}

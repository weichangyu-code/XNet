#include "WatchTimer.h"
#include "Utils.h"

namespace XNet
{
    WatchTimer::WatchTimer()
    {
        _clock = getMSClock();
    }

    unsigned int WatchTimer::howLong()
    {
        unsigned int prev = _clock;
        _clock = getMSClock();
        return _clock - prev;
    }

    void WatchTimer::call(unsigned int interval, std::function<void()> func)
    {
        unsigned int cur = getMSClock();
        if (cur - _clock > interval)
        {
            func();
            _clock = cur;
        }
    }

    bool WatchTimer::check(unsigned int interval)
    {
        unsigned int cur = getMSClock();
        if (cur - _clock > interval)
        {
            _clock = cur;
            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace XNet

#include "TimerManager.h"
#include "Utils.h"

namespace XNet
{
    TimerManager::TimerManager()
    {
    }
    
    TimerManager::~TimerManager()
    {
        stop();
    }

    void TimerManager::addTimer(unsigned int ms, const std::function<void()>& cb)
    {
        Guard g(_mtx);
        _timers.insert(make_pair(getMSClockEx() + ms, cb));
    }

    void TimerManager::run()
    {
        unsigned long long cur = getMSClockEx();
        while (_timers.empty() == false)
        {
            Guard g(_mtx);
            if (_timers.empty())
            {
                break;
            }
            auto it = _timers.begin();
            if (it->first > cur)
            {
                break;
            }
            
            auto cb = std::move(it->second);
            _timers.erase(it);
            g.unlock();
            
            //不要在cb后调用g.unlock()，等于cb在锁里面是否，容易导致死锁
            cb();
        }
    }

    void TimerManager::stop()
    {
        Guard g(_mtx);
        auto timers = std::move(_timers);
        g.unlock();

        for (auto& it : timers)
        {
            it.second();
        }
    }
    
} // namespace XNet

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
        _timers.insert({getMSClockEx() + ms, {cb, _timerIdMap.end()}});
    }
        
    void TimerManager::_addTimer(unsigned int ms, const std::function<void()>& cb, unsigned int& timerId)
    {
        if (timerId > 0)
        {
            _delTimer(timerId);
        }
        
        Guard g(_mtx);
        timerId = _getNewTimerId();

        auto it = _timers.insert({getMSClockEx() + ms, {cb}});
        it->second.it = _timerIdMap.insert({timerId, it}).first;
    }
        
    void TimerManager::_delTimer(unsigned int timerId)
    {
        Guard g(_mtx);
        auto it = _timerIdMap.find(timerId);
        if (it != _timerIdMap.end())
        {
            _timers.erase(it->second);
            _timerIdMap.erase(it);
        }
    }
        
    unsigned int TimerManager::_getNewTimerId()
    {
        unsigned int timerId = ++_timerIdSeek;
        if (timerId == 0)
        {
            timerId = ++_timerIdSeek;
        }
        return timerId;
    }
        
    // unsigned long long TimerManager::_getNewTimerId(unsigned int ms)
    // {
    //     unsigned long long timerId = (getMSClockEx() + ms) * 1000;
    //     unsigned long long timerIdEnd = timerId + 1000;

    //     while (_timers.empty() == false)
    //     {
    //         //找一个小于timeIdEnd的最大值
    //         unsigned long long timerIdMax = 0;
    //         auto it = _timers.lower_bound(timerIdEnd);
    //         if (it != _timers.begin())
    //         {
    //             if (it == _timers.end())
    //             {
    //                 timerIdMax = _timers.rbegin()->first;
    //             }
    //             else
    //             {
    //                 --it;
    //                 timerIdMax = it->first;
    //             }
    //         }
    //         if (timerIdMax < timerId)
    //         {
    //             break;
    //         }
    //         else if (timerIdMax < timerIdEnd - 1)
    //         {
    //             timerId = timerIdMax + 1;
    //             break;
    //         }

    //         //1000个用完成了
    //         timerId += 1000;
    //         timerIdEnd += 1000;
    //     }
    //     return timerId;
    // }

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
            
            auto timeInfo = std::move(it->second);
            if (timeInfo.it != _timerIdMap.end())
            {
                _timerIdMap.erase(timeInfo.it);
            }
            _timers.erase(it);
            g.unlock();
            
            //不要在cb后调用g.unlock()，等于cb在锁里面是否，容易导致死锁
            timeInfo.cb();
        }
    }

    void TimerManager::stop()
    {
        Guard g(_mtx);
        auto timers = std::move(_timers);
        _timerIdMap.clear();
        g.unlock();

        for (auto& it : timers)
        {
            it.second.cb();
        }
    }
        
    Timer::Timer(TimerManager* manager)
    {
        _manager = manager;
    }

    void Timer::start(unsigned int ms, const std::function<void()>& cb)
    {
        Guard g(_mtx);
        _manager->_addTimer(ms, cb, _timerId);
    }
        
    void Timer::stop()
    {
        Guard g(_mtx);
        if (_timerId > 0)
        {
            _manager->_delTimer(_timerId);
            _timerId = 0;
        }
    }
    
} // namespace XNet

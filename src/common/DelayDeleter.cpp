#include "DelayDeleter.h"
#include "CommonPublic.h"

namespace XNet
{
    DelayDeleter::DelayDeleter()
        :_heartbeat(-10000)                       //尽快碰到循环
    {
        
    }
    
    DelayDeleter::~DelayDeleter()
    {
        stop();
    }
        
    void DelayDeleter::add(const std::function<void()>& del)
    {
        Guard g(_mtx);
        _delQueue.push(make_pair(_heartbeat.load() + 30, del));
    }

    void DelayDeleter::runDelete()
    {
        auto v = _heartbeat.fetch_add(1);
        if (v % 10 == 0)
        {
            Guard g(_mtx);
            while (_delQueue.empty() == false)
            {
                auto it = _delQueue.front();
                int diff = (int)(_heartbeat - it.first);     //会循环，必须用int
                if (diff > 0 || diff < -10000)               //diff < 10000为了容错，其实没必要
                {
                    _delQueue2.push(it.second);
                    _delQueue.pop();
                }
                else
                {
                    break;
                }
            }
            g.unlock();
            
            while (_delQueue2.empty() == false)
            {
                auto& del = _delQueue2.front();
                del();
                _delQueue2.pop();
            }
        }
    }

    void DelayDeleter::stop()
    {
        Guard g(_mtx);
        auto delQueue = std::move(_delQueue);
        g.unlock();
        
        while (delQueue.empty() == false)
        {
            auto& del = delQueue.front().second;
            del();
            delQueue.pop();
        }
    }

    
} // namespace XNet

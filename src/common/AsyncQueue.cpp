#include "AsyncQueue.h"

namespace XNet
{
    AsyncQueue::AsyncQueue()
    {
    }
    
    AsyncQueue::~AsyncQueue()
    {
        stop();
    }
        
    void AsyncQueue::asyncEvent(const std::function<void()>& cb, bool active)
    {
        Guard g(_mtx);
        _asyncEvents.push(cb);
        g.unlock();
        
        if (active)
        {
            _cond.notify_one();
        }
    }
        
    void AsyncQueue::run(unsigned int timeout)
    {
        if (timeout == 0)
        {
            _executeAsyncEvents();
        }
        else
        {
            _executeAsyncEvents(timeout);
        }
    }
        
    void AsyncQueue::stop()
    {
        _executeAsyncEvents();
    }
        
    void AsyncQueue::_executeAsyncEvents()
    {
        Guard g(_mtx);
        while (_asyncEvents.empty() == false)
        {
            _bk.swap(_asyncEvents);
            g.unlock();
            
            while (_bk.empty() == false)
            {
                auto& event = _bk.front();
                event();
                _bk.pop();
            }

            g.lock();
        }
    }
        
    void AsyncQueue::_executeAsyncEvents(unsigned int timeout)
    {
        Guard g(_mtx);
        if (_asyncEvents.empty())
        {
            _cond.wait_for(g, std::chrono::milliseconds(timeout));
        }
        while (_asyncEvents.empty() == false)
        {
            _bk.swap(_asyncEvents);
            g.unlock();
            
            while (_bk.empty() == false)
            {
                auto& event = _bk.front();
                event();
                _bk.pop();
            }

            g.lock();
        }
    }
} // namespace XNet

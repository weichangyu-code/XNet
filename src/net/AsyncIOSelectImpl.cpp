#include "AsyncIOSelectImpl.h"
#include "WatchTimer.h"

#if defined(__WIN32__)

namespace XNet
{
    AsyncIOImpl::AsyncIOImpl()
    {
        
    }
    
    AsyncIOImpl::~AsyncIOImpl()
    {
    }
    
    bool AsyncIOImpl::start()
    {
        return true;
    }
        
    void AsyncIOImpl::stop()
    {
        
    }

    void AsyncIOImpl::registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat)
    {
        Guard g(_mtx);
        _operators.push({s, listener});
    }

    void AsyncIOImpl::unregisterEvent(SOCKET s)
    {
        Guard g(_mtx);
        _operators.push({s, nullptr});
    }
        
    void AsyncIOImpl::_runOperators()
    {
        Guard g(_mtx);
        _operators2.swap(_operators);
        g.unlock();

        while (_operators2.empty() == false)
        {
            auto& op = _operators2.front();

            if (op.second)
            {
                _listeners[op.first] = op.second;
            }
            else
            {
                _listeners.erase(op.first);
            }
            _operators2.pop();
        }
    }

    void AsyncIOImpl::run(unsigned int timeout, bool& andMore)
    {
        _runOperators();
        
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_ZERO(&exceptfds);

        for (auto& it : _listeners)
        {
			FD_SET(it.first, &readfds);
			FD_SET(it.first, &exceptfds);
            if (it.second->isListenSend())
            {
			    FD_SET(it.first, &writefds);
            }
        }

        struct timeval ts;
        ts.tv_sec = timeout / 1000;
        ts.tv_usec = (timeout % 1000) * 1000;
        int num = select(0, &readfds, &writefds, &exceptfds, &ts);
        if (num > 0)
        {
            for (int i = 0;i < (int)readfds.fd_count;i++)
            {
                auto it = _listeners.find(readfds.fd_array[i]);
                if (it != _listeners.end())
                {
                    it->second->onEventRead();
                }
            }
            
            for (int i = 0;i < (int)writefds.fd_count;i++)
            {
                auto it = _listeners.find(writefds.fd_array[i]);
                if (it != _listeners.end())
                {
                    it->second->onEventSend();
                }
            }
            
            for (int i = 0;i < (int)exceptfds.fd_count;i++)
            {
                auto it = _listeners.find(exceptfds.fd_array[i]);
                if (it != _listeners.end())
                {
                    it->second->onEventException();
                }
            }
        }
    }

} // namespace XNet

#endif


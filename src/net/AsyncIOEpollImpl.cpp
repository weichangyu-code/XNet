#include "AsyncIOEpollImpl.h"
#include "WatchTimer.h"

#if !defined(__MACOSX__) && !defined(__IPHONEOS__) && !defined(__APPLE__) && !defined(__WIN32__)

namespace XNet
{
    AsyncIOImpl::AsyncIOImpl()
    {
        _events.resize(1000);
    }
    
    AsyncIOImpl::~AsyncIOImpl()
    {
    }
    
    bool AsyncIOImpl::start()
    {
        stop();

		_epollFd = ::epoll_create(1);
        if (_epollFd < 0)
        {
            return false;
        }
        
        return true;
    }
        
    void AsyncIOImpl::stop()
    {
        if (_epollFd >= 0)
        {
            ::close(_epollFd);
            _epollFd = -1;
        }
    }

    void AsyncIOImpl::registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat)
    {
        if (_epollFd < 0)
        {
            return;
        }

        struct epoll_event ev;
        ev.data.ptr = listener;
        ev.events = EPOLLIN|EPOLLOUT;
        if (repeat == false)
        {
            ev.events |= EPOLLET;
        }
        ::epoll_ctl(_epollFd, EPOLL_CTL_ADD, s, &ev);
    }

    void AsyncIOImpl::unregisterEvent(SOCKET s)
    {
        if (_epollFd < 0)
        {
            return;
        }

        struct epoll_event ev = {0};
        ::epoll_ctl(_epollFd, EPOLL_CTL_DEL, s, &ev);
    }

    void AsyncIOImpl::run(unsigned int timeout, bool& andMore)
    {
        if (_epollFd < 0)
        {
            andMore = false;
            return;
        }

		int num = ::epoll_wait(_epollFd, _events.data(), _events.size(), timeout);
        for (int i = 0;i < num;i++)
        {
            struct epoll_event& event = _events[i];

            auto lsner = (AsyncIOListener*)event.data.ptr;
            if (lsner == nullptr)
            {
                continue;
            }
            
            if (event.events & EPOLLIN)
            {
                lsner->onEventRead();
            }
            if (event.events & EPOLLOUT)
            {
                lsner->onEventSend();
            }
			if ((event.events & EPOLLERR) || (event.events & EPOLLHUP))
            {
                lsner->onEventException();
            }
        }

        andMore = num == (int)_events.size();

        // //测试
        // static int count = 0;
        // static WatchTimer timer;
        // count++;
        // if (timer.check(1000))
        // {
        //     printf("count=%d\n", count);
        //     count = 0;
        // }
    }

} // namespace XNet

#endif

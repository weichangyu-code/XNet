#include "AsyncIOKQueueImpl.h"
#include "WatchTimer.h"


#if defined(__MACOSX__) || defined(__IPHONEOS__) || defined(__APPLE__)

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

		_kqueue = ::kqueue();
        if (_kqueue < 0)
        {
            return false;
        }
        
        return true;
    }
        
    void AsyncIOImpl::stop()
    {
        if (_kqueue >= 0)
        {
            ::close(_kqueue);
            _kqueue = -1;
        }
    }

    void AsyncIOImpl::registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat)
    {
        if (_kqueue < 0)
        {
            return;
        }

        struct kevent ev[3] = {0};
        EV_SET(&ev[0], s, EVFILT_READ, EV_ADD | (repeat ? 0 : EV_CLEAR), 0, 0, listener);
        EV_SET(&ev[1], s, EVFILT_WRITE, EV_ADD | (repeat ? 0 : EV_CLEAR), 0, 0, listener);
        // EV_SET(&ev[2], s, EVFILT_EXCEPT, EV_ADD | (repeat ? 0 : EV_CLEAR), 0, 0, listener);
        kevent(_kqueue, ev, 2, nullptr, 0, nullptr);
    }

    void AsyncIOImpl::unregisterEvent(SOCKET s)
    {
        if (_kqueue < 0)
        {
            return;
        }

        struct kevent ev[3] = {0};
        EV_SET(&ev[0], s, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
        EV_SET(&ev[1], s, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
        // EV_SET(&ev[2], s, EVFILT_EXCEPT, EV_DELETE, 0, 0, nullptr);
        kevent(_kqueue, ev, 2, nullptr, 0, nullptr);
    }

    void AsyncIOImpl::run(unsigned int timeout, bool& andMore)
    {
        if (_kqueue < 0)
        {
            andMore = false;
            return;
        }

        struct timespec ts;
        ts.tv_sec = timeout / 1000;
        ts.tv_nsec = (timeout % 1000) * 1000000;
        int num = kevent(_kqueue, nullptr, 0, _events.data(), _events.size(), &ts);
        for (int i = 0;i < num;i++)
        {
            struct kevent& event = _events[i];

            auto lsner = (AsyncIOListener*)event.udata;
            if (lsner == nullptr)
            {
                continue;
            }
            
            if (event.filter == EVFILT_READ)
            {
                lsner->onEventRead();
            }
            else if (event.filter == EVFILT_WRITE)
            {
                lsner->onEventSend();
            }
			else if (event.filter == EVFILT_EXCEPT)
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


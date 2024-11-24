#pragma once
#include "Network.h"
#include "AsyncIOListener.h"

namespace XNet
{
    class AsyncIOImpl
    {
    public:
        AsyncIOImpl();
        ~AsyncIOImpl();

    public:
        bool start();
        void stop();

        void run(unsigned int timeout, bool& andMore);

        void registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat);
        void unregisterEvent(SOCKET s);

    protected:
        int _epollFd = -1;

        vector<struct epoll_event> _events;
    };

    typedef shared_ptr<AsyncIOImpl> AsyncIOImplPtr;
    
} // namespace XNet

#pragma once
#include "Network.h"
#include "AsyncIOListener.h"

#if defined(__MACOSX__) || defined(__IPHONEOS__) || defined(__APPLE__)

#include <sys/event.h>

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
        int _kqueue = -1;

        vector<struct kevent> _events;
    };

    typedef shared_ptr<AsyncIOImpl> AsyncIOImplPtr;
    
} // namespace XNet

#endif

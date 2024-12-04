#pragma once

#include "Network.h"
#include "AsyncIOListener.h"

#if defined(__WIN32__)

#ifdef FD_SETSIZE
#undef FD_SETSIZE
#endif
#define FD_SETSIZE  40000

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
        void _runOperators();

    protected:
        mutex _mtx;
        queue<pair<SOCKET, AsyncIOListener*>> _operators;
        queue<pair<SOCKET, AsyncIOListener*>> _operators2;

        unordered_map<SOCKET, AsyncIOListener*> _listeners;
        
        //作为临时变量用
		fd_set readfds;
		fd_set writefds;
		fd_set exceptfds;
    };

    typedef shared_ptr<AsyncIOImpl> AsyncIOImplPtr;
    
} // namespace XNet

#endif

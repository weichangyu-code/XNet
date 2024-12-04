#pragma once
#include "NetPublic.h"
#include "AsyncIOListener.h"
#include "AsyncQueue.h"
#include "TimerManager.h"
#include "DelayDeleter.h"
#include "AsyncIOKQueueImpl.h"
#include "AsyncIOEpollImpl.h"
#include "AsyncIOIOCPImpl.h"
#include "AsyncIOSelectImpl.h"

namespace XNet
{
    class AsyncIO
    {
    public:
        AsyncIO();
        ~AsyncIO();

    public:
        bool start(int threadNum);
        void stop();

        void registerEvent(void* obj, AsyncIOListener* lsner, SOCKET sock, bool repeat);
        void unregisterEvent(void* obj, SOCKET sock);
        void delayDelete(void* obj, function<void()> del);

        void asyncEvent(void* obj, const function<void()>& cb);                 //asyncEvent不保证实时性
        void addTimer(void* obj, unsigned int ms, const function<void()>& cb);

    protected:
        int _getThreadIndex(void* obj);

    protected:
        volatile bool _running = false;

        struct AsyncIOThread
        {
            AsyncIOImplPtr io;
            shared_ptr<AsyncQueue> ioQueue;
            // shared_ptr<AsyncQueue> eventQueue;
            shared_ptr<TimerManager> timer;
            thread ioThread;
            // thread eventThread;
            thread::id ioThreadId;
            thread::id eventThreadId;
            shared_ptr<DelayDeleter> delayDeleter;
        };
        vector<AsyncIOThread> _threads;
    };
    
} // namespace XNet


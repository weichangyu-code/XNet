#pragma once
#include "CommonPublic.h"

namespace XNet
{
    class AsyncQueue
    {
    public:
        AsyncQueue();
        ~AsyncQueue();

        void asyncEvent(const std::function<void()>& cb, bool active);
        void run(unsigned int timeout);
        void stop();

    protected:
        void _executeAsyncEvents();
        void _executeAsyncEvents(unsigned int timeout);
        
    protected:
        std::mutex _mtx;
        std::condition_variable _cond;
        std::queue<std::function<void()>> _asyncEvents;
        std::queue<std::function<void()>> _bk;
    };
} // namespace XNet

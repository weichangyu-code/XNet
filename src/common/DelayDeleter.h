#pragma once
#include <functional>
#include <mutex>
#include <queue>
#include <atomic>

namespace XNet
{
    class DelayDeleter
    {
    public:
        DelayDeleter();
        ~DelayDeleter();
        
    public:
        void add(const std::function<void()>& del);

        void runDelete();
        void stop();

    protected:
        std::mutex _mtx;
        std::atomic_uint _heartbeat;
        std::queue<std::pair<unsigned int, std::function<void()>>> _delQueue;
        std::queue<std::function<void()>> _delQueue2;
    };
}
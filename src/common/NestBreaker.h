#pragma once
#include "CommonPublic.h"

namespace XNet
{
    //破解嵌套调用，变成循环调用
    class NestBreaker
    {
    public:
        NestBreaker();
        
    public:
        void execute(std::function<void()> f);

    protected:
        std::atomic_int _nestNum;
        
        std::mutex _mtx;
        std::queue<std::function<void()>> _queue;
    };
}

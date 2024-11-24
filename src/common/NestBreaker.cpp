#include "NestBreaker.h"

namespace XNet
{
    NestBreaker::NestBreaker()
        :_nestNum(0)
    {

    }
    
    void NestBreaker::execute(std::function<void()> f)
    {
        while (f)
        {
            int oldValue = _nestNum.fetch_add(1);
            if (oldValue == 0)
            {
                f();
                f = nullptr;
            }
            else
            {
                Guard g(_mtx);
                _queue.push(std::move(f));
            }

            int newValue = _nestNum.fetch_sub(1) - 1;
            if (newValue == 0)
            {
                Guard g(_mtx);
                if (_queue.empty() == false)
                {
                    f = std::move(_queue.front());
                    _queue.pop();
                }
            }
        }
    }
}
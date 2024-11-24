#include "AsyncIO.h"
#include "Utils.h"
#include "NetStatistics.h"

namespace XNet
{
    
    AsyncIO::AsyncIO()
    {
    }
    
    AsyncIO::~AsyncIO()
    {
        stop();
    }
        
    bool AsyncIO::start(int threadNum)
    {
        _running = true;

        for (int i = 0; i < threadNum; i++)
        {
            AsyncIOThread asyncIOThread;
            
            asyncIOThread.io = make_shared<AsyncIOImpl>();
            if (asyncIOThread.io->start() == false)
            {
                return false;
            }
            asyncIOThread.ioQueue = make_shared<AsyncQueue>();
            // asyncIOThread.eventQueue = make_shared<AsyncQueue>();
            asyncIOThread.timer = make_shared<TimerManager>();
            asyncIOThread.delayDeleter = make_shared<DelayDeleter>();

            _threads.push_back(std::move(asyncIOThread));
        }
        
        for (int i = 0; i < threadNum; i++)
        {
            auto& asyncIOThread = _threads[i];
            asyncIOThread.ioThread = thread([this, &asyncIOThread, i]() {
                setThreadName("XNetIOThread-%d", i);
                
                asyncIOThread.ioThreadId = this_thread::get_id();
                while (_running)
                {
                    bool andMore = false;
                    asyncIOThread.io->run(30, andMore);
                    asyncIOThread.timer->run();
                    
                    //队列放最后，确保前面模块和自己扔异步队列可以快速响应
                    asyncIOThread.ioQueue->run(0);

                    asyncIOThread.delayDeleter->runDelete();

                    NetStatistics::threadRunNum++;

                    //控制唤醒频率，优化CPU
                    if (andMore == false)
                    {
                        sleepMS(1);
                    }
                }
            });
            
            // asyncIOThread.eventThread = thread([this, &asyncIOThread, i]() {
            //     setThreadName("XNetEventThread-%d", i);
                
            //     asyncIOThread.eventThreadId = this_thread::get_id();
            //     while (_running)
            //     {
            //         asyncIOThread.eventQueue->run(100);
                    
            //         //控制唤醒频率，优化CPU
            //         sleepMS(1);
            //     }
            // });
        }

        return true;
    }
        
    void AsyncIO::stop()
    {
        _running = false;

        //等待线程退出
        for (auto& asyncIOThread : _threads)
        {
            // asyncIOThread.eventThread.join();
            asyncIOThread.ioThread.join();
        }

        //清理数据
        for (auto& asyncIOThread : _threads)
        {
            asyncIOThread.io->stop();
            // asyncIOThread.eventQueue->stop();
            asyncIOThread.ioQueue->stop();
            asyncIOThread.timer->stop();
            asyncIOThread.delayDeleter->stop();
        }
        _threads.clear();
    }
        
    int AsyncIO::_getThreadIndex(void* obj)
    {
        return ((unsigned int)(uintptr_t)obj / 53) % _threads.size();
    }

    void AsyncIO::registerEvent(void* obj, AsyncIOListener* lsner, SOCKET sock, bool repeat)
    {
        auto& asyncIOThread = _threads[_getThreadIndex(obj)];
        asyncIOThread.io->registerEvent(lsner, sock, repeat);
    }

    void AsyncIO::unregisterEvent(void* obj, SOCKET sock)
    {
        auto& asyncIOThread = _threads[_getThreadIndex(obj)];
        asyncIOThread.io->unregisterEvent(sock);
    }
        
    void AsyncIO::delayDelete(void* obj, function<void()> del)
    {
        auto& asyncIOThread = _threads[_getThreadIndex(obj)];
        asyncIOThread.delayDeleter->add(del);
    }
        
    void AsyncIO::asyncEvent(void* obj, const function<void()>& cb)
    {
        auto& asyncIOThread = _threads[_getThreadIndex(obj)];
        // if (this_thread::get_id() == asyncIOThread.ioThreadId)
        {
            asyncIOThread.ioQueue->asyncEvent(cb, false);
        }
        // else if (this_thread::get_id() == asyncIOThread.eventThreadId)
        // {
        //     asyncIOThread.eventQueue->asyncEvent(cb, false);
        // }
        // else
        // {
        //     asyncIOThread.eventQueue->asyncEvent(cb, true);
        // }
    }

    void AsyncIO::addTimer(void* obj, unsigned int ms, const function<void()>& cb)
    {
        auto& asyncIOThread = _threads[_getThreadIndex(obj)];
        asyncIOThread.timer->addTimer(ms, cb);
    }
}

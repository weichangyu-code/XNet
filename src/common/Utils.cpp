//
// Created by lance on 2023/8/23.
//
#include "Utils.h"
#include <chrono>
#include <thread>
#include <sys/stat.h>
#include <fstream>
#include <memory.h>
#include <unistd.h>
#include <stdarg.h>

#ifndef __APPLE__
#include<sys/prctl.h>
#endif

namespace XNet
{

    unsigned int getTime()
    {
        return ::time(nullptr);
    }

    unsigned int getMSClock()
    {
        //return std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
        return std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
    }
    
    unsigned long long getMSClockEx()
    {
        return std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;    
    }

    void sleepMS(unsigned int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void sleepForever()
    {
        while (1)
        {
            sleepMS(-1);
        }
    }
    
    void setThreadName(const char* name, ...)
    {
        char bufName[64];
        bufName[sizeof(bufName) - 1] = 0;
        va_list va;
        va_start(va, name);
        vsnprintf(bufName, sizeof(bufName) - 1, name, va);
        va_end(va);

#if defined(__MACOSX__) || defined(__IPHONEOS__) || defined(__APPLE__)
	    pthread_setname_np(bufName);
#else
	    prctl(PR_SET_NAME, (unsigned long)bufName);
#endif
    }

    //修改无效
    // void setThreadPriority(int priority)
    // {
    //     struct sched_param sched_param = {0};
    //     int sched_policy = 0;
    //     int ret = pthread_getschedparam(pthread_self(), &sched_policy, &sched_param);
    //     sched_param.sched_priority = priority;
    //     pthread_setschedparam(pthread_self(), sched_policy, &sched_param);
    // }
}
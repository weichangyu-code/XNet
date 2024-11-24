#pragma once
#include "CommonPublic.h"

namespace XNet
{

    //获取时间戳
    unsigned int getMSClock();              //有环路的可能
    unsigned long long getMSClockEx();      //无环路
    unsigned int getTime();

    //休眠
    void sleepMS(unsigned int ms);
    void sleepForever();

    //设置线程名称
    void setThreadName(const char* name, ...);

    //内存栅栏
	#define compilerBarrier()	asm volatile("" ::: "memory")
}
#pragma once
#include "NetPublic.h"

namespace XNet
{
    class NetStatistics
    {
    public:
        static atomic_int tcpLinkNum;
        static atomic_int bufferSize;
        static unsigned int threadRunNum;
    };
} // namespace XNet

#include "NetStatistics.h"

namespace XNet
{
    atomic_int NetStatistics::tcpLinkNum(0);
    atomic_int NetStatistics::bufferSize(0);
    unsigned int NetStatistics::threadRunNum = 0;
    
} // namespace XNet

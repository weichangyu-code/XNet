#include "NetCore.h"
#include "Network.h"
#include "NetAPI.h"
#include "net/NetStatistics.h"
#include "common/Log.h"
#include "Utils.h"

namespace XNet
{
    NetCore::NetCore()
    {
        _asyncIO = make_shared<AsyncIO>();
    }
    
    NetCore::~NetCore()
    {
        stop();
    }

    bool NetCore::start(int threadNum)
    {
		signal(SIGPIPE, SIG_IGN);
        LOGI("sizeof(void*)=%d;__cplusplus=%d;clock=%lld", sizeof(void*), __cplusplus, getMSClockEx());
        
        if (_asyncIO->start(threadNum) == false)
        {
            return false;
        }

        LOGI("NetCore start ok.threadNum=%d", threadNum);
        return true;
    }
    void NetCore::stop()
    {
        _asyncIO->stop();
    }

    ITCPServerPtr NetCore::createTCPServer()
    {
        return TCPServer::create(_asyncIO.get());
    }
    
    ITCPLinkPtr NetCore::createTCPLink()
    {
        return TCPLink::create(_asyncIO.get());
    }
        
    void NetCore::getStatistics(NetCoreStatistics& stat)
    {
        stat.tcpLinkNum = NetStatistics::tcpLinkNum;
        stat.bufferSize = NetStatistics::bufferSize;
    }
    
    void NetCore::dump()
    {
        printf(
        "NetCore:\n"
        "   tcpLinkNum=%d\n"
        "   bufferSize=%d\n"
        "   threadRunNum=%d\n"
        , NetStatistics::tcpLinkNum.load()
        , NetStatistics::bufferSize.load()
        , NetStatistics::threadRunNum);

        NetStatistics::threadRunNum = 0;
    }
    
} // namespace XNet
#pragma once
#include "TCPServer.h"
#include "TCPLink.h"
#include "AsyncIO.h"
#include "INetCore.h"

namespace XNet
{
    class NetCore : public INetCore
    {
    public:
        NetCore();
        virtual ~NetCore();

    public:
        virtual bool start(int threadNum);
        virtual void stop();

        virtual ITCPServerPtr createTCPServer();
        virtual ITCPLinkPtr createTCPLink();

        virtual void getStatistics(NetCoreStatistics& stat);
        virtual void dump();

    protected:
        shared_ptr<AsyncIO> _asyncIO;
    };
    
} // namespace XNet


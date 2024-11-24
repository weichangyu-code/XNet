#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <string>
#include "ITCPLink.h"
#include "ITCPServer.h"

namespace XNet
{
    struct NetCoreStatistics
    {
        unsigned int tcpLinkNum;
        unsigned int bufferSize;
    };

    class INetCore;
    typedef std::shared_ptr<INetCore> INetCorePtr;
    class INetCore
    {
    public:
        virtual ~INetCore();

    public:
        static INetCorePtr create();
        static void getHostByName(const char* host, const std::function<void(bool, const char*, const std::vector<const char*>&)>& cb);
        
        virtual bool start(int threadNum) = 0;
        virtual void stop() = 0;

        virtual ITCPServerPtr createTCPServer() = 0;
        virtual ITCPLinkPtr createTCPLink() = 0;

        virtual void getStatistics(NetCoreStatistics& stat) = 0;
        virtual void dump() = 0;
    };
    
} // namespace XNet

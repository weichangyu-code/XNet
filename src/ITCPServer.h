#pragma once
#include <memory>
#include <functional>

namespace XNet
{
    class ITCPServer
    {
    public:
        virtual ~ITCPServer() {};

    public:
        virtual bool listen(const char* localIP, unsigned short port, bool reuseAddr, std::function<void()> onAccept) = 0;
        virtual void close() = 0;
        virtual bool isListening() const = 0;
    };
    
    typedef std::shared_ptr<ITCPServer> ITCPServerPtr;
} // namespace XNet

#pragma once
#include <memory>
#include <functional>
#include "ITCPServer.h"

namespace XNet
{
    struct TCPLinkCB
    {
        std::function<void(bool)> onConnect;
        std::function<void()> onClose;
        std::function<void()> onRecv;        //有新数据过来，而且数据要取完
        std::function<void()> onSend;        //发送缓冲从满到有空闲时回调
    };
    
    class ITCPLink
    {
    public:
        virtual ~ITCPLink() {};

    public:
        virtual bool connect(const char* remoteIP, unsigned short port, unsigned int timeout, const TCPLinkCB& cb) = 0;
        virtual bool accept(const ITCPServerPtr& server, const TCPLinkCB& cb) = 0;
        virtual void close() = 0;

        virtual bool sendAsync(const char* data, unsigned int size, const std::function<void(bool)>& onComplete) = 0;       //onComplete有嵌套的可能。data需要等到onComplete返回后才能释放。
        virtual bool send(const char* data, unsigned int size) = 0;                                                         //data可以直接释放，内部没有发送完成的会另申请缓冲区
        virtual unsigned int getSendQueueSize() const = 0;

        virtual bool haveRecvData() = 0;
        virtual int  recv(char* data, unsigned int size) = 0;

        virtual bool isConnected() const = 0;
        virtual const char* getRemoteIP() const = 0;
        virtual int  getRemotePort() const = 0;

        virtual void setSocketSendBuffer(unsigned int size) = 0;
        virtual void setSocketRecvBuffer(unsigned int size) = 0;
        
        virtual void asyncEvent(const std::function<void()>& cb) = 0;                    //asyncEvent不保证实时性
        virtual void addTimer(unsigned int ms, const std::function<void()>& cb) = 0;
    };
    
    typedef std::shared_ptr<ITCPLink> ITCPLinkPtr;
} // namespace XNet

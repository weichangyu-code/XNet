#pragma once
#include "NetPublic.h"
#include "AsyncIO.h"
#include "TCPServer.h"
#include "NestBreaker.h"
#include "ITCPLink.h"

namespace XNet
{
    class TCPLink;
    typedef shared_ptr<TCPLink> TCPLinkPtr;

    class TCPServer;
    class TCPLink : public ITCPLink, public enable_shared_from_this<TCPLink>, public AsyncIOListener
    {
    protected:
        TCPLink(AsyncIO* io);
        virtual ~TCPLink();

    public:
        virtual void onEventRead();
        virtual void onEventSend();
        virtual void onEventException();
        virtual void onDelayDelete();

    public:
        static TCPLinkPtr create(AsyncIO* io);

        virtual bool connect(const char* remoteIP, unsigned short port, unsigned int timeout, const TCPLinkCB& cb);
        virtual bool accept(const ITCPServerPtr& server, const TCPLinkCB& cb);
        virtual void close();

        virtual bool sendAsync(const char* data, unsigned int size, const function<void(bool)>& onComplete);        //onComplete异步释放内存用，有嵌套风险，不要做业务处理
        virtual bool send(const char* data, unsigned int size);                                                     //data可以直接释放，内部没有发送完成的会另申请缓冲区
        virtual unsigned int getSendQueueSize() const;

        virtual bool haveRecvData();
        virtual int  recv(char* data, unsigned int size);

        virtual bool isConnected() const;
        virtual const char* getRemoteIP() const;
        virtual int  getRemotePort() const;

        virtual void setSocketSendBuffer(unsigned int size);
        virtual void setSocketRecvBuffer(unsigned int size);
        
        virtual void asyncEvent(const function<void()>& cb);
        virtual void addTimer(unsigned int ms, const function<void()>& cb);

    protected:
        void _connect(const char* remoteIP, unsigned short port);
        bool _send(const char* data, unsigned int size, const function<void(bool)>& onComplete);
        
    protected:
        mutable mutex _mtx;
        AsyncIO* _io = nullptr;
        SOCKET _sock = -1;
        std::unique_ptr<TCPLinkCB> _cb;

        //状态
        enum
        {
            STATE_IDLE,
            STATE_DOMAIN_RESOLVING,
            STATE_CONNECTING,
            STATE_CONNECTED,
        }_state = STATE_IDLE;
        int _connectCount = 0;
        sockaddr_in _remoteAddr = {0};
        char _remoteIP[64] = {0};

        //发送
        struct SendData
        {
            const char* data;
            unsigned int size;
            function<void(bool)> onComplete;
        };
        queue<SendData> _sendQueue;
        queue<function<void(bool)>> _sendCompleteQueue;
        unsigned int _sendQueueSize = 0;

        //接收
        bool _haveRecvData = false;
    };

} // namespace XNet

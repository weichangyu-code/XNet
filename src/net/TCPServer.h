#pragma once
#include "NetPublic.h"
#include "AsyncIO.h"
#include "ITCPServer.h"

namespace XNet
{
    class TCPServer;
    typedef shared_ptr<TCPServer> TCPServerPtr;

    class TCPServer : public ITCPServer, public enable_shared_from_this<TCPServer>, public AsyncIOListener
    {
    protected:
        TCPServer(AsyncIO* io);
        virtual ~TCPServer();

    public:
        virtual void onEventRead();
        virtual void onEventSend();
        virtual void onEventException();
        virtual bool isListenSend();
        virtual void onDelayDelete();

    public:
        static TCPServerPtr create(AsyncIO* io);

        virtual bool listen(const char* localIP, unsigned short port, bool reuseAddr, function<void()> onAccept);
        virtual void close();
        virtual bool isListening() const;

        SOCKET accept(sockaddr_in* remoteAddr);
        
    protected:
        mutable mutex _mtx;
        AsyncIO* _io = nullptr;
        SOCKET _sock = INVALID_SOCKET;
        function<void()> _onAccept;
    };
} // namespace XNet

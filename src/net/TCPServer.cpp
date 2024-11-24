#include "TCPServer.h"
#include "NetAPI.h"

namespace XNet
{
    TCPServer::TCPServer(AsyncIO* io)
    {
        _io = io;
    }

    TCPServer::~TCPServer()
    {
        close();
    }
    
    TCPServerPtr TCPServer::create(AsyncIO* io)
    {
        TCPServer* tcpServer = new TCPServer(io);
        return TCPServerPtr(tcpServer, [](TCPServer* p) { p->onDelayDelete(); });
    }
        
    SOCKET TCPServer::accept(sockaddr_in* remoteAddr)
    {
        Guard g(_mtx);
        if (_sock == INVALID_SOCKET)
        {
            return INVALID_SOCKET;
        }

        socklen_t saddr_len = sizeof(*remoteAddr);
        SOCKET newSock = ::accept(_sock, (sockaddr*)remoteAddr, &saddr_len);
        if (newSock == INVALID_SOCKET)
        {
            return newSock;
        }

        return newSock;
    }
        
    void TCPServer::onEventRead()
    {
        Guard g(_mtx);
        if (_sock == INVALID_SOCKET)
        {
            return;
        }
        auto onAccept = _onAccept;
        g.unlock();

        if (onAccept)
        {
            onAccept();
        }
    }
    
    void TCPServer::onEventSend()
    {
        
    }
    
    void TCPServer::onEventException()
    {
        
    }
    
    void TCPServer::onDelayDelete()
    {
        close();
        
        _io->delayDelete(this, [this]() {
            delete this;
        });
    }

    bool TCPServer::listen(const char* localIP, unsigned short port, bool reuseAddr, function<void()> onAccept)
    {
        Guard g(_mtx);
        while (_sock != INVALID_SOCKET)
        {
            g.unlock();
            close();
            g.lock();
        }
        
        _sock = createTCPSocket();
        if (_sock == INVALID_SOCKET)
        {
            return false;
        }
        setSocketAsync(_sock);
        if (reuseAddr)
        {
            setSocketReuseaddr(_sock, true);
        }

        sockaddr_in saddr;
        memset(&saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(port);
        saddr.sin_addr = getIPAddress(localIP);
        
        if (::bind(_sock, (const struct sockaddr*)&saddr, sizeof(saddr)) != 0)
        {
            g.unlock();
            close();
            LOGW("bind ip:%s port:%d failed", localIP, port);
            return false;
        }

        if (::listen(_sock, 128) != 0)
        {
            g.unlock();
            close();
            LOGW("listen ip:%s port:%d failed", localIP, port);
            return false;
        }
        LOGI("listen ip:%s port:%d success", localIP, port);

        _onAccept = onAccept;
        _io->registerEvent(this, this, _sock, true);
        return true;
    }

    void TCPServer::close()
    {
        Guard g(_mtx);
        if (_sock != INVALID_SOCKET)
        {
            _io->unregisterEvent(this, _sock);
            
            ::close(_sock);
            _sock = INVALID_SOCKET;
            
            function<void()> onAccept = std::move(_onAccept);
            g.unlock();
            
            //回调销毁放到锁外面，避免死锁
            onAccept = nullptr;
        }
    }
    
    bool TCPServer::isListening() const
    {
        Guard g(_mtx);
        return _sock != INVALID_SOCKET;
    }

    
    
    
}
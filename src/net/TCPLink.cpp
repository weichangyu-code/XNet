#include "TCPLink.h"
#include "NetAPI.h"
#include "Buffer.h"
#include "DomainResolver.h"
#include "Log.h"
#include "NetStatistics.h"
#include "Utils.h"
#include "NetAPI.h"

namespace XNet
{
    TCPLink::TCPLink(AsyncIO* io)
    {
        NetStatistics::tcpLinkNum++;

        _io = io;
    }

    TCPLink::~TCPLink()
    {
        assert(_sock == INVALID_SOCKET);
        
        NetStatistics::tcpLinkNum--;
        //LOGI("TCPLink::~TCPLink count=%d", g_linkCount.fetch_sub(1) - 1);
    }
    
    TCPLinkPtr TCPLink::create(AsyncIO* io)
    {
        TCPLink* tcpLink = new TCPLink(io);
        return TCPLinkPtr(tcpLink, [](TCPLink* p) { p->onDelayDelete(); });
    }

    void TCPLink::onEventRead()
    {
        _haveRecvData = true;

        auto cb = _cb.get();
        if (cb && cb->onRecv)
        {
            cb->onRecv();
        }
    }
        
    void TCPLink::onEventSend()
    {
        //进行二次判断
        if (_state == STATE_CONNECTED)
        {
            if (_sendQueueSize > 0)
            {
                Guard g(_mtx);
                while (_sendQueue.empty() == false && _state == STATE_CONNECTED)
                {
                    SendData& sendData = _sendQueue.front();

                    int ret = ::send(_sock, sendData.data, sendData.size, 0);
                    if (ret == -1)
                    {
                        ret = 0;
                    }

                    _sendQueueSize -= ret;
                    if (ret < (int)sendData.size)
                    {
                        sendData.data += ret;
                        sendData.size -= ret;
                        break;
                    }

                    //发送成功
                    _sendCompleteQueue.push(std::move(sendData.onComplete));
                    _sendQueue.pop();
                }
                bool haveSendData = _sendQueueSize > 0;
                g.unlock();
                
                while (_sendCompleteQueue.empty() == false)
                {
                    _sendCompleteQueue.front()(true);
                    _sendCompleteQueue.pop();
                }

                if (haveSendData)
                {
                    //还有数据为发送完成，等下次的消息
                    return;
                }
            }

            auto cb = _cb.get();
            if (cb && cb->onSend)
            {
                cb->onSend();
            }
        }
        else if (_state == STATE_CONNECTING)
        {
            Guard g(_mtx);
            if (_state == STATE_CONNECTING)
            {
                int err = 0;
                getSocketOption(_sock, SOL_SOCKET, SO_ERROR, (char*)&err, sizeof(err));
                if (err == 0)
                {
                    //登录成功
                    _state = STATE_CONNECTED;
                    function<void(bool)> onConnect = std::move(_cb->onConnect);
                    g.unlock();
                    
                    if (onConnect)
                    {
                        onConnect(true);
                    }
                    onEventSend();
                }
                else
                {
                    g.unlock();
                    
                    onEventException();
                }
            }
        }
    }
    
    void TCPLink::onEventException()
    {
        Guard g(_mtx);
        if (_state != STATE_IDLE && _state != STATE_CONNECTED)
        {
            function<void(bool)> onConnect = std::move(_cb->onConnect);
            g.unlock();
            
            close();
            if (onConnect)
            {
                onConnect(false);
            }
        }
        else if (_state == STATE_CONNECTED)
        {
            function<void()> onClose = std::move(_cb->onClose);
            g.unlock();
            
            close();
            if (onClose)
            {
                onClose();
            }
        }
    }
    
    bool TCPLink::isListenSend()
    {
        return _state == STATE_CONNECTING || _sendQueue.empty() == false;
    }
    
    void TCPLink::onDelayDelete()
    {
        close();
        
        _io->delayDelete(this, [this]() {
            delete this;
        });
    }

    bool TCPLink::connect(const char* remoteIP, unsigned short port, unsigned int timeout, const TCPLinkCB& cb)
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
        
        _cb.reset(new TCPLinkCB(cb));
        
        auto this_ = shared_from_this();
        int connectCount = ++_connectCount;

        if (isIPv4(remoteIP))
        {
            _state = STATE_CONNECTING;
            _connect(remoteIP, port);
        }
        else
        {
            _state = STATE_DOMAIN_RESOLVING;
            DomainResolver::getHostByName(remoteIP, [this_, connectCount, port](bool success, const char* ip, const vector<string>& ips) {
                Guard g(this_->_mtx);
                if (this_->_state == STATE_DOMAIN_RESOLVING && this_->_connectCount == connectCount)
                {
                    if (success && ips.empty() == false)
                    {
                        this_->_state = STATE_CONNECTING;
                        this_->_connect(ips.front().c_str(), port);
                    }
                    else
                    {
                        g.unlock();
                        this_->onEventException();
                    }
                }
            });
        }

        _io->addTimer(this, timeout, [this_, connectCount](){
            Guard g(this_->_mtx);
            if (this_->_state != STATE_IDLE
                && this_->_state != STATE_CONNECTED
                && this_->_connectCount == connectCount)
            {
                g.unlock();
                this_->onEventException();
            }
        });
        
        return true;
    }
        
    void TCPLink::_connect(const char* remoteIP, unsigned short port)
    {
        strcpy(_remoteIP, remoteIP);
        memset(&_remoteAddr, 0, sizeof(_remoteAddr));
        _remoteAddr.sin_family = AF_INET;
        _remoteAddr.sin_port = htons(port);
        _remoteAddr.sin_addr = getIPAddress(remoteIP);
        ::connect(_sock, (sockaddr*)&_remoteAddr, sizeof(_remoteAddr));
        _io->registerEvent(this, this, _sock, false);
    }
        
    bool TCPLink::accept(const ITCPServerPtr& server, const TCPLinkCB& cb)
    {
        const TCPServerPtr server2 = static_pointer_cast<TCPServer>(server);
        
        //非递归锁，需要这么处理
        Guard g(_mtx);
        while (_sock != INVALID_SOCKET)
        {
            g.unlock();
            close();
            g.lock();
        }
    
        memset(&_remoteAddr, 0, sizeof(_remoteAddr));
        _sock = server2->accept(&_remoteAddr);
        if (_sock == INVALID_SOCKET)
        {
            return false;
        }
        setSocketAsync(_sock);
        strcpy(_remoteIP, getIPString(_remoteAddr.sin_addr).c_str());
        
        _state = STATE_CONNECTED;
        _cb.reset(new TCPLinkCB(cb));
        _io->registerEvent(this, this, _sock, false);
        return true;
    }

    void TCPLink::close()
    {
        Guard g(_mtx);
        if (_sock != INVALID_SOCKET)
        {
            _io->unregisterEvent(this, _sock);
            
            ::closesocket(_sock);
            _sock = INVALID_SOCKET;
            _state = STATE_IDLE;
            
            //放到IO线程处理，这样IO线程访问的时候不用加锁
            //auto cb = std::move(_cb);
            _io->asyncEvent(this, [this_ = shared_from_this()]()
            {
                Guard g(this_->_mtx);
                if (this_->_state == STATE_IDLE)
                {
                    auto cb = std::move(this_->_cb);
                    g.unlock();

                    //避免死锁
                    cb.reset();
                }
            });

            //发送数据
            queue<SendData> sendQueue = std::move(_sendQueue);
            _sendQueueSize = 0;

            //接收数据
            _haveRecvData = false;

            g.unlock();

            //清除发送队列
            while (sendQueue.empty() == false)
            {
                auto& sendData = sendQueue.front();
                sendData.onComplete(false);
                sendQueue.pop();
            }
        }
    }

    bool TCPLink::isConnected() const
    {
        // Guard g(_mtx);
        return _state == STATE_CONNECTED;
    }
    
    const char* TCPLink::getRemoteIP() const
    {
        return _remoteIP;
    }
    
    int TCPLink::getRemotePort() const
    {
        return ntohs(_remoteAddr.sin_port);
    }
    
    unsigned int TCPLink::getSendQueueSize() const
    {
        // Guard g(_mtx);
        return _sendQueueSize;
    }

    bool TCPLink::sendAsync(const char* data, unsigned int size, const function<void(bool)>& onComplete)
    {
        if (onComplete == nullptr)
        {
            return false;
        }
        return _send(data, size, onComplete);
    }
        
    bool TCPLink::send(const char* data, unsigned int size)
    {
        return _send(data, size, nullptr);
    }
    
    bool TCPLink::haveRecvData()
    {
        // Guard g(_mtx);
        return _haveRecvData;
    }

    int TCPLink::recv(char* data, unsigned int size)
    {
        if (data == nullptr || size == 0)
        {
            return -1;
        }
        
        Guard g(_mtx);
        _haveRecvData = false;                      //提前设置，避免recv空后再修改，这样无法确保onRecv提前回调
        compilerBarrier();
        if (_state == STATE_CONNECTED)
        {
            int ret = ::recv(_sock, data, size, 0);
            if (ret == 0 || (ret == -1 && (errno == ECONNRESET || errno == EPIPE)))
            {
                //取消注册，不会再有回调
                _io->unregisterEvent(this, _sock);
                
                //异步回调
                _io->asyncEvent(this, [this_ = shared_from_this()]()
                {
                    this_->onEventException();
                });
                
                return -1;
            }
            if (ret == -1)
            {
                ret = 0;
            }

            if (ret == (int)size)
            {
                //数据已经收完
                _haveRecvData = true;
            }

            return ret;
        }
        else if (_state == STATE_IDLE)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
        
    bool TCPLink::_send(const char* data, unsigned int size, const function<void(bool)>& onComplete)
    {
        if (data == nullptr || size == 0)
        {
            return false;
        }
        
        Guard g(_mtx);
        if (_state == STATE_IDLE)
        {
            g.unlock();
            if (onComplete)
            {
                onComplete(false);
            }
            return false;
        }

        _sendQueueSize += size;                         //提前设置，这样可以在OnEventSend不用加锁判断是否超限
        compilerBarrier();

        unsigned int sendLen = 0;
        if (_sendQueue.empty())
        {
            if (_state == STATE_CONNECTED)
            {
                int ret = ::send(_sock, data, size, 0);
                if (ret == -1)
                {
                    ret = 0;
                }
                _sendQueueSize -= ret;
                sendLen = ret;
                if (ret == (int)size)
                {
                    //发送成功
                    g.unlock();
                    if (onComplete)
                    {
                        onComplete(true);
                    }
                    return true;
                }
            }
            else
            {
                //正在连接，数据放到发送队列
                // int break_ = 1;
            }
        }

        SendData sendData;
        sendData.size = size - sendLen;
        if (onComplete == nullptr)
        {
            BufferPtr buf = Buffer::create(data + sendLen, sendData.size);
            sendData.data = buf->data();
            sendData.onComplete = [buf](bool success){
                //让buf等调用完成后释放
            };
        }
        else
        {
            sendData.data = data + sendLen;
            sendData.onComplete = onComplete;
        }
        _sendQueue.push(sendData);
        return true;
    }
        
    void TCPLink::setSocketSendBuffer(unsigned int size)
    {
        Guard g(_mtx);
        if (_sock != INVALID_SOCKET)
        {
            setSocketSendBuf(_sock, size);
        }
    }
        
    void TCPLink::setSocketRecvBuffer(unsigned int size)
    {
        Guard g(_mtx);
        if (_sock != INVALID_SOCKET)
        {
            setSocketRecvBuf(_sock, size);
        }
    }
    
    void TCPLink::asyncEvent(const function<void()>& cb)
    {
        _io->asyncEvent(this, cb);
    }
    
    void TCPLink::addTimer(unsigned int ms, const function<void()>& cb)
    {
        _io->addTimer(this, ms, cb);
    }
    
}
#include "NetAPI.h"
#include "Log.h"

namespace XNet
{    
    SOCKET createTCPSocket()
    {
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    void setSocketAsync(SOCKET sock)
    {
    #if defined(WIN32)
        unsigned long l = 1;
        ioctlsocket(sock, FIONBIO, &l);
    #else
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    #endif
    }

    void setSocketOption(SOCKET sock, int level, int type, void* value, unsigned int valueLen)
    {
        int ret = ::setsockopt(sock, level, type, (char*)value, valueLen);
        if (ret < 0)
        {
            LOGW("setsockopt %d error %d\n", type, ret);
        }
    }

    void getSocketOption(SOCKET sock, int level, int type, void* value, unsigned int valueLen)
    {
        ::getsockopt(sock, level, type, (char*)value, (socklen_t*)&valueLen);
    }
        
    void setSocketLinger(SOCKET sock, int value)
    {
        struct linger data;
        data.l_onoff = (value > 0) ? 1 : 0;
        data.l_linger = value;
        setSocketOption(sock, SOL_SOCKET, SO_LINGER, &data, sizeof(data));
    }
        
    void setSocketNodelay(SOCKET sock, bool value)
    {
        int optval = value ? 1 : 0;
        setSocketOption(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
    }
        
    void setSocketSendBuf(SOCKET sock, unsigned int size)
    {
        setSocketOption(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
    }

    void setSocketRecvBuf(SOCKET sock, unsigned int size)
    {
        setSocketOption(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
    }
    
    unsigned int getSocketSendBuf(SOCKET sock)
    {
        unsigned int size = 0;
        getSocketOption(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
        return size;
    }
    
    unsigned int getSocketRecvBuf(SOCKET sock)
    {
        unsigned int size = 0;
        getSocketOption(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
        return size;
    }

    void setSocketReuseaddr(SOCKET sock, bool value)
    {
        int data = value;
        setSocketOption(sock, SOL_SOCKET, SO_REUSEADDR, &data, sizeof(data));
    }
    
    in_addr getIPAddress(const char* ip)
    {
        in_addr addr = {0};
        inet_pton(AF_INET, ip, &addr);
        return addr;
    }
    string  getIPString(in_addr addr)
    {
        char buf[16] = {0};
        inet_ntop(AF_INET, &addr, buf, sizeof(buf));
        return buf;
    }

    bool isIPv4(const char* ip)
    {
        int len = strlen(ip);
        if (len > 15)
        {
            return false;
        }
        for (int i = 0; i < len; i++)
        {
            char ch = ip[i];
            if (ch >= '0' && ch <= '9')
            {
                continue;
            }
            if (ch == '.')
            {
                continue;
            }
            return false;
        }
        return true;
    }
}
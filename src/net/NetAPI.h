#pragma once
#include "NetPublic.h"

namespace XNet
{
    SOCKET createTCPSocket();
    void setSocketAsync(SOCKET sock);
    void setSocketOption(SOCKET sock, int level, int type, void* value, unsigned int valueLen);
    void getSocketOption(SOCKET sock, int level, int type, void* value, unsigned int valueLen);
    void setSocketLinger(SOCKET sock, int value);
    void setSocketNodelay(SOCKET sock, bool value);
    void setSocketSendBuf(SOCKET sock, unsigned int size);
    void setSocketRecvBuf(SOCKET sock, unsigned int size);
    unsigned int getSocketSendBuf(SOCKET sock);
    unsigned int getSocketRecvBuf(SOCKET sock);
    void setSocketReuseaddr(SOCKET sock, bool value);
    
    in_addr getIPAddress(const char* ip);
    string  getIPString(in_addr addr);

    bool isIPv4(const char* ip);
} // namespace XNet

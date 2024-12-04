#include "net/Network.h"
#include "net/NetPublic.h"
#include "net/NetAPI.h"
#include <sys/epoll.h>
#include "common/Utils.h"
#include "common/Log.h"
using namespace std;
using namespace XNet;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 0;
    }
    int port = atoi(argv[1]);

    int epollFd = ::epoll_create(1);
    
    int listenSocket = createTCPSocket();
    setSocketAsync(listenSocket);
    setSocketReuseaddr(listenSocket, true);

    sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr = getIPAddress("0.0.0.0");
    
    if (::bind(listenSocket, (const struct sockaddr*)&saddr, sizeof(saddr)) != 0)
    {
        LOGW("bind port:%d failed", port);
        return 0;
    }
    ::listen(listenSocket, 128);
    LOGW("listen port:%d success", port);

    struct epoll_event ev;
    ev.data.fd = listenSocket;
    ev.events = EPOLLIN|EPOLLOUT;
    ::epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSocket, &ev);

    int clientSocket = -1;
    while (true)
    {
        const unsigned int EVENT_NUM = 100;
		struct epoll_event events[EVENT_NUM];

		int num = ::epoll_wait(epollFd, events, EVENT_NUM, 100);
        for (int i = 0;i < num;i++)
        {
            struct epoll_event& event = events[i];

            if (event.data.fd == listenSocket)
            {
                if (event.events & EPOLLIN)
                {
                    clientSocket = accept(listenSocket, NULL, NULL);
                    if (clientSocket < 0)
                    {
                        LOGW("accept failed");
                        continue;
                    }
                    LOGW("accept success");
                    setSocketAsync(clientSocket);
                    
                    struct epoll_event ev;
                    ev.data.fd = clientSocket;
                    ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
                    ::epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &ev);
                }
            }
            else if (event.data.fd == clientSocket)
            {
                if (event.events & EPOLLIN)
                {
                    while (true)
                    {
                        char buf[16*1024];
                        int len = ::recv(clientSocket, buf, sizeof(buf), 0);
                        if (len > 0)
                        {
                            ::send(clientSocket, buf, len, 0);
                        }
                        if (len < sizeof(buf))
                        {
                            break;
                        }
                    }
                }
            }
        }
        XNet::sleepMS(1);
    }
}
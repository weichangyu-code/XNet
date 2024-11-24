#include "INetCore.h"
#include "common/Crash.h"
#include "common/Utils.h"
#include <mutex>
#include <vector>
#include <atomic>
#include <thread>
using namespace std;
using namespace XNet;
int main(int argc, char** argv)
{
    //测试下原子操作和锁的性能
    // atomic_int a(0);

    // printf("line=%d time=%u\n", __LINE__, getMSClock());
    // for (int i = 0;i < 100000000;i++)
    // {
    //     a.fetch_add(1);
    // }
    // printf("line=%d time=%u\n", __LINE__, getMSClock());

    // mutex mtx;
    // for (int i = 0;i < 100000000;i++)
    // {
    //     mtx.lock();
    //     mtx.unlock();
    // }
    // printf("line=%d time=%u\n", __LINE__, getMSClock());
    // return 0;

    setupHandleCrash();

    if (argc < 4)
    {
        printf("Usage: %s <ip> <port> <linkNum>\n", argv[0]);
        return 0;
    }
    
    INetCorePtr netCore = INetCore::create();
    mutex linksMtx;
    vector<ITCPLinkPtr> links;
    netCore->start(1);

    atomic_uint sendLen(0);
    atomic_uint recvLen(0);

    function<void(const ITCPLinkPtr&)> onClose = [&](const ITCPLinkPtr& link){
        std::unique_lock<std::mutex> g(linksMtx);
        for (auto iter = links.begin();iter != links.end();++iter)
        {
            if (*iter == link)
            {
                links.erase(iter);
                break;
            }
        }
        printf("onClose links=%d\n", (int)links.size());
    };
    
    function<void(const ITCPLinkPtr&)> onSend = [&](const ITCPLinkPtr& link){
        while (link->isConnected() && link->getSendQueueSize() == 0)
        {
            char data[32*1024];
            for (int i = 0;i < sizeof(data);i++)
            {
                data[i] = i;
            }
            link->send(data, sizeof(data));
            sendLen.fetch_add(sizeof(data));
        }
    };

    function<void(const ITCPLinkPtr&)> onRecv = [&](const ITCPLinkPtr& link){
        while (link->haveRecvData())
        {
            char buf[32*1024];
            int len = link->recv(buf, sizeof(buf));
            if (len > 0)
            {
                // printf("recvLen=%u time=%u\n", len, getMSClock());

                recvLen.fetch_add(len);
            }
        }
    };

    function<void(const ITCPLinkPtr&, bool)> onConnect = [&](const ITCPLinkPtr& link, bool success) {
        if (success)
        {
            printf("onConnect link=%p success=%d\n", link.get(), success);
        }
        else
        {
            std::unique_lock<std::mutex> g(linksMtx);
            for (auto iter = links.begin();iter != links.end();++iter)
            {
                if (*iter == link)
                {
                    links.erase(iter);
                    break;
                }
            }
            printf("onConnect failed links=%d\n", (int)links.size());
        }
    };

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int linkNum = atoi(argv[3]);
    for (int i = 0;i < linkNum;i++)
    {
        auto link = netCore->createTCPLink();
        
        std::unique_lock<std::mutex> g(linksMtx);
        links.push_back(link);
        g.unlock();

        TCPLinkCB cb;
        cb.onConnect = bind(onConnect, link, std::placeholders::_1);
        cb.onClose = bind(onClose, link);
        cb.onRecv = bind(onRecv, link);
        cb.onSend = bind(onSend, link);
        
        link->connect(ip, port, 10*1000, cb);
        
        // link->connect("192.168.147.130", 22, 10*1000, bind(onConnect, link, std::placeholders::_1), bind(onClose, link), bind(onRead, link));
        //link->connect("8.2.5.6", 22 + 1, 10*1000, bind(onConnect, link, std::placeholders::_1), bind(onClose, link), bind(onRead, link));
    }

    while (true)
    {

        // std::unique_lock<std::mutex> g(linksMtx);
        // if (links.empty() == false)
        // {
        //     auto link = links.front();
        //     char buf[1024];
        //     link->send(buf, sizeof(buf));
        //     printf("sendLen=%u time=%u\n", (unsigned int)sizeof(buf), getMSClock());
        // }
        // g.unlock();

        this_thread::sleep_for(chrono::milliseconds(1000));
        printf("sendLen=%dMB recvLen=%dMB\n", (int)(sendLen.load() / 1024 / 1024), (int)(recvLen.load() / 1024 / 1024));
        sendLen.store(0);
        recvLen.store(0);
        netCore->dump();
    }

    getchar();
    return 0;
}

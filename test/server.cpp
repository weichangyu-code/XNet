#include "INetCore.h"
#include "common/Crash.h"
#include "common/Utils.h"
#include "net/TCPLink.h"
#include <mutex>
#include <vector>
#include <atomic>
#include <thread>
using namespace std;
using namespace XNet;


void readAndSend(const ITCPLinkPtr& link)
{
    while (link->getSendQueueSize() == 0 && link->haveRecvData())
    {
        char buf[16*1024];
        int len = link->recv(buf, sizeof(buf));
        if (len > 0)
        {
            link->send(buf, len);
        }
    }
}

int main(int argc, char** argv)
{
    setupHandleCrash();

    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 0;
    }
    
    INetCorePtr netCore = INetCore::create();
    mutex linksMtx;
    vector<ITCPLinkPtr> links;
    netCore->start(1);
    
    auto server = netCore->createTCPServer();
    
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

    
    function<void(const ITCPLinkPtr&, bool)> onSend = [&](const ITCPLinkPtr& link, bool success){
        if (success)
        {
            readAndSend(link);
        }
    };

    function<void(const ITCPLinkPtr&)> onRecv = [&](const ITCPLinkPtr& link){
        // char buf[128] = {0};
        // link->recv(buf, sizeof(buf) - 1);
        // printf("onRead %s\n", buf);
        // link->send(buf, strlen(buf));

        // unsigned int bufLen = 128*1024;
        // shared_ptr<char> buf2(new char[bufLen]);
        // for (int i = 0;i < bufLen;i++)
        // {
        //     buf2.get()[i] = 'a';
        // }
        // link->sendAsync(buf2.get(), bufLen, [buf2](bool success){
        //     success = true;
        // });
        
        readAndSend(link);
    };

    TCPLink* linkPtr = nullptr;
    auto onAccept = [&]() {
        auto link = netCore->createTCPLink();

        TCPLinkCB cb;
        cb.onClose = bind(onClose, link);
        cb.onRecv = bind(onRecv, link);
        cb.onSend = bind(onSend, link, true);
        
        if (link->accept(server, cb))
        {
            std::unique_lock<std::mutex> g(linksMtx);
            links.push_back(link);
            linkPtr = (TCPLink*)link.get();
            printf("onAccept links=%d\n", (int)links.size());
        }
    };

    int port = atoi(argv[1]);
    server->listen("", port, true, onAccept);
    
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1000));
        netCore->dump();
    }

    while (true)
    {
        sleepMS(1000);
        netCore->dump();
    }

    getchar();
    return 0;
}

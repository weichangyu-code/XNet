#include "DomainResolver.h"
#include "NetAPI.h"

namespace XNet
{
    
    DomainResolver::DomainResolver()
    {
    }
    
    DomainResolver::~DomainResolver()
    {
    }
    
    void DomainResolver::getHostByName(const char* host, const function<void(bool, const char*, const vector<string>&)>& cb)
    {
        string host2 = host;
        thread([host2, cb](){
            if (isIPv4(host2.c_str()))
            {
                cb(true, host2.c_str(), {host2});
                return;
            }

            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_flags = AI_PASSIVE;
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            struct addrinfo* res0 = nullptr;
            int error = getaddrinfo(host2.c_str(), 0, &hints, &res0);
            if (error != 0) 
            {
                cb(false, host2.c_str(), {});
                return;
            }

            vector<string> ips;
            for (auto res = res0; res; res = res->ai_next)
            {
                if(res->ai_family != AF_INET)
                    continue;
                    
                sockaddr_in6 addrv6;
                memset(&addrv6, 0, sizeof(addrv6));
                memcpy(&addrv6, res->ai_addr, res->ai_addrlen);
                sockaddr_in* pAddr = (sockaddr_in*)&addrv6;
                
                ips.push_back(getIPString(pAddr->sin_addr));
            }
            freeaddrinfo(res0);

            cb(true, host2.c_str(), ips);
        }).detach();
    }

} // namespace XNet

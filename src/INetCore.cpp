#include "INetCore.h"
#include "net/NetCore.h"
#include "net/DomainResolver.h"

namespace XNet
{
    INetCore::~INetCore()
    {
    }

    INetCorePtr INetCore::create()
    {
        return make_shared<NetCore>();
    }
    
    void INetCore::getHostByName(const char* host, const std::function<void(bool, const char*, const std::vector<const char*>&)>& cb)
    {
        DomainResolver::getHostByName(host, [cb](bool success, const char* ip, const vector<string>& ips)
        {
            std::vector<const char*> ips2;
            for (auto& ip : ips)
            {
                ips2.push_back(ip.c_str());
            }
            cb(success, ip, ips2);
        });
    }
} // namespace XNet

#pragma once
#include "NetPublic.h"

namespace XNet
{
    class DomainResolver
    {
    public:
        DomainResolver();
        ~DomainResolver();

    public:
        static void getHostByName(const char* host, const function<void(bool, const char*, const vector<string>&)>& cb);
    };
    
} // namespace XNet

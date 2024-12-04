#pragma once
#include "NetPublic.h"

namespace XNet
{
    class AsyncIOListener
    {
    public:
        virtual void onEventRead() = 0;
        virtual void onEventSend() = 0;
        virtual void onEventException() = 0;

        virtual bool isListenSend() = 0;
    };
}

// #pragma once
// #include "Network.h"
// #include "AsyncIOListener.h"

// #if defined(__WIN32__)

// namespace XNet
// {
//     class AsyncIOImpl
//     {
//     public:
//         AsyncIOImpl();
//         ~AsyncIOImpl();

//     public:
//         bool start();
//         void stop();

//         void run(unsigned int timeout, bool& andMore);

//         void registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat);
//         void unregisterEvent(SOCKET s);

//     protected:
//         HANDLE _iocp = NULL;
//     };

//     typedef shared_ptr<AsyncIOImpl> AsyncIOImplPtr;
    
// } // namespace XNet

// #endif

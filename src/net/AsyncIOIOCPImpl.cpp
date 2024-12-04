// #include "AsyncIOIOCPImpl.h"
// #include "WatchTimer.h"

// #if defined(__WIN32__)

// #pragma comment(lib, "ws2_32.lib")

// namespace XNet
// {
//     AsyncIOImpl::AsyncIOImpl()
//     {
        
//     }
    
//     AsyncIOImpl::~AsyncIOImpl()
//     {
//     }
    
//     bool AsyncIOImpl::start()
//     {
//         stop();

//         _iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
//         if (_iocp == NULL)
//         {
//             return false;
//         }


        
//         return true;
//     }
        
//     void AsyncIOImpl::stop()
//     {
//         if (_iocp != NULL)
//         {
// 			PostQueuedCompletionStatus(_iocp, 0, 0, 0);
            
// 			::CloseHandle(_iocp);
// 			_iocp = NULL;
//         }
//     }

//     void AsyncIOImpl::registerEvent(AsyncIOListener* listener, SOCKET s, bool repeat)
//     {
// 		::CreateIoCompletionPort((HANDLE)s, _iocp, (ULONG_PTR)0, 0);
//     }

//     void AsyncIOImpl::unregisterEvent(SOCKET s)
//     {
        
//     }

//     void AsyncIOImpl::run(unsigned int timeout, bool& andMore)
//     {
        
//     }

// } // namespace XNet

// #endif


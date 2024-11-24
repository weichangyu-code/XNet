#include "Crash.h"
#include <signal.h>
#include <stdio.h>

#ifdef __x86_64__
#include <execinfo.h>
#endif

namespace XNet
{
    static void handleCrash(int sig)
    {
        printf("handleCrash %d\n", sig);
        dumpBacktrace();
    }
    
    void dumpBacktrace()
    {
#ifdef __x86_64__
        void* buf[1024];
        int nptrs = backtrace(buf, 1024);
        char **strings = backtrace_symbols(buf, nptrs);
        for (int i = 0; i < nptrs; i++) 
        {
            printf("%s\n", strings[i]);
        }
#endif
    }
    
    void setupHandleCrash()
    {
#ifdef __x86_64__
        signal(SIGSEGV, handleCrash);
        signal(SIGABRT, handleCrash);
        signal(SIGFPE, handleCrash);
        signal(SIGILL, handleCrash);
        signal(SIGBUS, handleCrash);
        signal(SIGTRAP, handleCrash);
#endif
    }
} // namespace XNet

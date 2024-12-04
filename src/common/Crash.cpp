#include "Crash.h"
#include "Log.h"
#include <signal.h>
#include <stdio.h>

#if defined(__x86_64__) && defined(__linux__)
#include <execinfo.h>
#endif

namespace XNet
{
    static void handleCrash(int sig)
    {
        LOGW("handleCrash %d\n", sig);
        dumpBacktrace();
        exit(1);
    }
    
    void dumpBacktrace()
    {
#if defined(__x86_64__) && defined(__linux__)
        void* buf[1024];
        int nptrs = backtrace(buf, 1024);
        char **strings = backtrace_symbols(buf, nptrs);
        for (int i = 0; i < nptrs; i++) 
        {
            LOGW("%s\n", strings[i]);
        }
        free(strings);
#endif
    }
    
    void setupHandleCrash()
    {
#if defined(__x86_64__) && defined(__linux__)
        signal(SIGSEGV, handleCrash);
        signal(SIGABRT, handleCrash);
        signal(SIGFPE, handleCrash);
        signal(SIGILL, handleCrash);
        signal(SIGBUS, handleCrash);
        signal(SIGTRAP, handleCrash);
#endif
    }
} // namespace XNet

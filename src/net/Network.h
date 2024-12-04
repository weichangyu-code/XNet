#pragma once

#ifdef __WIN32__

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x600)
    #undef _WIN32_WINNT
    #define _WIN32_WINNT    0x600
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>		/* TCP_NODELAY */
#include <arpa/inet.h>	/* inet(3) functions */
#include <sys/types.h>
#include <fcntl.h>		/* for nonblocking */
#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

typedef int SOCKET;
#define INVALID_SOCKET -1

#define closesocket close

#endif



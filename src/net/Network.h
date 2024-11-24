#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>		/* TCP_NODELAY */
#include <arpa/inet.h>	/* inet(3) functions */
#include <sys/types.h>
#include <fcntl.h>		/* for nonblocking */
#include <errno.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

typedef int SOCKET;
#define INVALID_SOCKET -1


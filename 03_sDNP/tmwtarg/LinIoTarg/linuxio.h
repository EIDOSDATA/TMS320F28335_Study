/*
	Linux header file for Linux support
*/

#ifndef LINUXIO_H
#define LINUXIO_H

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <signal.h>

#define SOCKET_ERROR        -1
#define INVALID_SOCKET      -1

#define MAKE_SOCKET_NON_BLOCKING( s, retval ) \
    { \
        struct sigaction sa; \
        memset( &sa, 0, sizeof(sa )); \
        sa.sa_handler = SIG_IGN; \
        sigaction( SIGPIPE, &sa, (struct sigaction *) NULL ); \
	      retval = fcntl( s, F_SETFL, O_NONBLOCK ); \
    }

#define MAKE_SOCKET_REUSEADDR( s, ret ) \
    { \
        int cmd_arg = 1; \
        ret = setsockopt( s, SOL_SOCKET, SO_REUSEADDR,  \
                (char *) &cmd_arg, sizeof(cmd_arg) ); \
    }

#define MAKE_SOCKET_KEEPALIVE( s, ret ) \
    { \
        int cmd_arg = 1; \
        ret = setsockopt( s, SOL_SOCKET, SO_KEEPALIVE,  \
                (char *) &cmd_arg, sizeof(cmd_arg) ); \
    }

#define MAKE_SOCKET_NO_DELAY( s, ret ) \
    { \
        int cmd_arg = 1; \
        ret = setsockopt( s, SOL_TCP, TCP_NODELAY,  \
                (char *) &cmd_arg, sizeof(cmd_arg) ); \
    }

#endif  // LINUXIO_H

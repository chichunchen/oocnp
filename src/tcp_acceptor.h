#ifndef __TCPACCEPTOR_H
#define __TCPACCEPTOR_H

#include <string.h>
#include <ds.h>
#include "unp.h"
#include "object.h"
#include "tcp_stream.h"

#define MAX_CONN_NUM    500000
#define INIT_CLIENT_SIZE 1000

typedef enum STATE {
    INITIAL, CONNECTING, FINISH
} state;

typedef struct TCPAcceptor {
    Object proto;
    int     state;
    int     listenfd;
    // clients -> infd
    char    *ip;
    char    *port;

    /* functions */
    int  (*listen)(void *self);
    void (*epoll_loop)(void *self, void (*welcome)(TCPStream*), void (*handle)(TCPStream*));
    void (*close)(void *self);
} TCPAcceptor;

Object TCPAcceptorProto;

#endif

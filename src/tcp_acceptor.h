#ifndef __TCPACCEPTOR_H
#define __TCPACCEPTOR_H

#include <string.h>
#include "unp.h"
#include "object.h"

typedef enum STATE {
    INITIAL, CONNECTING, FINISH
} state;

typedef struct TCPAcceptor {
    Object proto;
    int     state;
    int     sockfd;
    char    *ip;
    char    *port;

    /* functions */
    int (*listen)(void *self);
    void (*epoll_loop)(void *self, void (*cb1)(void), void (*cb2)(void));
    void (*close)(void *self);
} TCPAcceptor;

Object TCPAcceptorProto;

#endif

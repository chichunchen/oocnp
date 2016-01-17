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
    //
} TCPAcceptor;

Object TCPAcceptorProto;
int TCPAcceptor_listen();

#endif

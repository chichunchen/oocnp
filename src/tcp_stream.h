#ifndef __TCPSTREAM_H
#define __TCPSTREAM_H

#include "unp.h"
#include "object.h"

#define IP_LEN   1024
#define PORT_LEN 20

typedef struct TCPStream {
    Object proto;
    int sockfd;
    char peerIP[IP_LEN];
    char peerPort[PORT_LEN];

    /* functions */
    void (*send)(void *self, char *buffer, size_t len);
    ssize_t (*receive)(void *self, char *buffer, size_t len);
} TCPStream;

void * TCPStream_init(void *self, int sd, struct sockaddr address);

#endif

#include "tcp_stream.h"
#include "tcp_acceptor.h"

/* private */

ssize_t                     /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;       /* and call write() again */
            else
                return(-1);         /* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

ssize_t                     /* Readn "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nread;
    char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) <= 0) {
            if (nread < 0 && errno == EINTR)
                nread = 0;       /* and call read() again */
            else
                return(-1);         /* error */
        }

        nleft -= nread;
        ptr   += nread;
    }
    return(n);
}

/* public */

void TCPStream_send(void * self, char *buffer, size_t len)
{
    TCPStream *tcp_stream   = self;
    if (writen(tcp_stream->sockfd, buffer, len) != len)
        log_err("writen error");
}

void TCPStream_receive(int fd, char *buffer, size_t len)
{
    if (readn(fd, buffer, len) != len)
        log_err("readn error");
}

void * TCPStream_init(void *self, int sd, struct sockaddr address)
{
    TCPStream *tcp_stream   = self;
    tcp_stream->sockfd = sd;
    tcp_stream->send = TCPStream_send;
    tcp_stream->receive = TCPStream_receive;

    socklen_t in_len = sizeof address;
    int s;

    s = getnameinfo(&address, in_len,
            tcp_stream->peerIP, IP_LEN-1,
            tcp_stream->peerPort, PORT_LEN-1,
            NI_NUMERICHOST | NI_NUMERICSERV);

    if (s == 0) {
        log_info("Accepted connection on descriptor %d "
                "(host=%s, port=%s)\n",
                tcp_stream->sockfd, tcp_stream->peerIP, tcp_stream->peerPort);
    }
    return tcp_stream;
}

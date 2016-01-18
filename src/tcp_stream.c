#include "tcp_stream.h"

ssize_t TCPStream_send(char *buffer, size_t len)
{

}

ssize_t TCPStream_receive(char *buffer, size_t len)
{
}

void * TCPStream_init(void *self, int sd, struct sockaddr address)
{
    TCPStream *tcp_stream   = self;
    tcp_stream = malloc(sizeof(TCPStream));
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

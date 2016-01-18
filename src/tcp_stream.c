#include "tcp_stream.h"
#include "tcp_acceptor.h"

/* private */

static ssize_t writen(int fd, const void *vptr, size_t n)
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

static ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			if (n == 1)
				return(0);	/* EOF, no data read */
			else
				break;		/* EOF, some data was read */
		} else
			return(-1);	/* error */
	}

	*ptr = 0;
	return(n);
}

/* public */

void TCPStream_send(void * self, char *buffer, size_t len)
{
    TCPStream *tcp_stream   = self;
    if (writen(tcp_stream->sockfd, buffer, len) != len)
        log_err("writen error");
}

ssize_t TCPStream_receive(void *self, char *buffer, size_t len)
{
    TCPStream *tcp_stream   = self;
	ssize_t  	n;
	int fd    = tcp_stream->sockfd;

	if ((n = readline(fd, buffer, len)) == -1)
		log_err("receive error");

	return (n);
}

void * TCPStream_init(void *self, int sd, struct sockaddr address)
{
    TCPStream *tcp_stream   = self;
    tcp_stream->sockfd = sd;
	/* connect function proto */
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

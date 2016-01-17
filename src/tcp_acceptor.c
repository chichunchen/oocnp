#include "tcp_acceptor.h"

/* private */
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
    int     n;

again:
    if ( (n = accept(fd, sa, salenptr)) < 0) {
#ifdef  EPROTO
        if (errno == EPROTO || errno == ECONNABORTED)
#else
        if (errno == ECONNABORTED)
#endif
            goto again;
        else {
            perror("[ERROR] Server - accept error");
            exit(0);
        }
    }
    return(n);
}

void SetReuseSock(int listenfd)
{
    int optval = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
        perror("[ERROR] Server - setsockopt");
    }
}

/* public */

int create_and_bind(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */

    s = getaddrinfo (NULL, port, &hints, &result);
    if (s != 0)
    {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        SetReuseSock(sfd);
        s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
        if (s == 0)
        {
            /* We managed to bind
             * successfully! */
            break;
        }

        close (sfd);
    }

    if (rp == NULL)
    {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo (result);

    return sfd;
}


int make_socket_non_blocking(int sockfd)
{
    int flags, s;

    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sockfd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl set");
        return -1;
    }
    return 0;
}

void * TCPAcceptor_init(void *self)
{
    TCPAcceptor *tcp_acceptor = self;
    tcp_acceptor->sockfd = 0;
    tcp_acceptor->port = "11111";

    tcp_acceptor->sockfd = create_and_bind(tcp_acceptor->port);

    make_socket_non_blocking(tcp_acceptor->sockfd);

    listen(tcp_acceptor->sockfd, LISTENQ);

    return tcp_acceptor;
}

Object TCPAcceptorProto = {
    .init = TCPAcceptor_init
};

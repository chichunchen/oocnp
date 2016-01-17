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

/* public */

int TCPAcceptor_listen(void *self)
{
    TCPAcceptor *tcp_acceptor = self;
    tcp_acceptor->sockfd = create_and_bind(tcp_acceptor->port);

    make_socket_non_blocking(tcp_acceptor->sockfd);

    listen(tcp_acceptor->sockfd, LISTENQ);

    return 0;
}

void TCPAcceptor_epoll_loop(void *self, void (*cb1)(void), void (*cb2)(void))
{
    TCPAcceptor *tcp_acceptor = self;
    int     efd, s;
    struct epoll_event event;
    struct epoll_event *events;

    efd = epoll_create1(0);
    if (efd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    event.data.fd = tcp_acceptor->sockfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd, EPOLL_CTL_ADD, tcp_acceptor->sockfd, &event);
    if (s == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    events = calloc(10000, sizeof(event));

    while(1) {
        int n, i;

        n = epoll_wait(efd, events, 10000, -1);
        for (i = 0; i < n; i++) {

            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN)))
            {
                /* An error has occured on this fd, or the
                 * socket is not ready for reading (why
                 * were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            }
            else if (tcp_acceptor->sockfd == events[i].data.fd)
            {
                /* We have a notification on the listening socket, which
                 * means one or more incoming connections */
                while(1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[1024], sbuf[20];

                    in_len = sizeof in_addr;
                    infd = accept(tcp_acceptor->sockfd, &in_addr, &in_len);
                    if (infd == -1) {
                        if ((errno == EAGAIN) ||
                            (errno == EWOULDBLOCK))
                        {
                            /* We have processed all incoming connections */
                            break;
                        }
                        else
                        {
                            perror("accept");
                            break;
                        }
                    }

                    s = getnameinfo(&in_addr, in_len,
                            hbuf, sizeof hbuf,
                            sbuf, sizeof sbuf,
                            NI_NUMERICHOST | NI_NUMERICSERV);
                    if (s == 0) {
                        log_info("Accepted connection on descriptor %d "
                               "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                    /* Make the incoming socket non-blocking and add it to
                     * list of fds to monitor. */
                    s = make_socket_non_blocking(infd);
                    if (s == -1) {
                        log_err("non block");
                        exit(EXIT_FAILURE);
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if (s == -1) {
                        perror("epoll_ctl");
                        exit(EXIT_FAILURE);
                    }

                    /* handle new client */
                    log_info("new client!!");
                    cb1();
                }
                continue;
            } // end of accept routine
            else {

                cb2();
//                 int c = 0;
// 
//                 for (c = 0; c < CLIENT_MAX_NUM; c++) {
//                     client = clients_get(c);
//                     client_socket = client->socket;
// 
//                     if (events[i].data.fd == client_socket) {
//                         //log_info("client_id=%d, client_socket=%d", c, client_socket);
// 
//                         /* callback */
// 
//                         break;
//                     }
//                 }
            }
        }
    }
}

void TCPAcceptor_close(void *self)
{
    TCPAcceptor *tcp_acceptor = self;
    int rc = close(tcp_acceptor->sockfd);
    if (rc != 0) {
        perror("[ERROR] close fail");
    }
    return;
}

void * TCPAcceptor_init(void *self)
{
    TCPAcceptor *tcp_acceptor = self;
    tcp_acceptor->sockfd = 0;
    tcp_acceptor->port = "7777";
    tcp_acceptor->listen = TCPAcceptor_listen;
    tcp_acceptor->epoll_loop = TCPAcceptor_epoll_loop;
    tcp_acceptor->close = TCPAcceptor_close;

    return tcp_acceptor;
}

Object TCPAcceptorProto = {
    .init = TCPAcceptor_init
};

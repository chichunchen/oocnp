#include <unistd.h>
#include "tcp_acceptor.h"

void welcome(void *tcp_acceptor)
{
    ssize_t n;
    char buf[100];
    //n = write(tcp_acceptor->listenfd, );
}

void handle(void * tcp_acceptor)
{
    log_info("handle");
}

int main(int argc, const char *argv[])
{
    TCPAcceptor *tcp_acceptor = NEW(TCPAcceptor, "ggg");
    tcp_acceptor->listen(tcp_acceptor);
    printf("listen for port=%s, listenfd=%d\n", tcp_acceptor->port, tcp_acceptor->listenfd);

    tcp_acceptor->epoll_loop(tcp_acceptor, welcome, handle);

    return 0;
}

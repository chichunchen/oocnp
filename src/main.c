#include "tcp_acceptor.h"

void welcome()
{
    log_info("hi");
}

void handle()
{
    log_info("handle");
}

int main(int argc, const char *argv[])
{
    TCPAcceptor *tcp_acceptor = NEW(TCPAcceptor, "ggg");
    tcp_acceptor->listen(tcp_acceptor);
    printf("listen for port=%s, sockfd=%d\n", tcp_acceptor->port, tcp_acceptor->sockfd);

    tcp_acceptor->epoll_loop(tcp_acceptor, welcome, handle);

    return 0;
}

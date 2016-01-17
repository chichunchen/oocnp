#include "tcp_acceptor.h"

int main(int argc, const char *argv[])
{
    TCPAcceptor *tcp_acceptor = NEW(TCPAcceptor, "ggg");
    printf("listen for port=%s, sockfd=%d\n", tcp_acceptor->port, tcp_acceptor->sockfd);

    while(1)
    {
    }

    return 0;
}

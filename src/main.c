#include <unistd.h>
#include "tcp_stream.h"
#include "tcp_acceptor.h"

void welcome(TCPStream *tcp_stream)
{
    char buf[10] = "hihihi";
    tcp_stream->send(tcp_stream, buf, 10);
}

void handle(TCPStream *tcp_stream)
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

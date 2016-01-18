#include "tcp_stream.h"
#include "tcp_acceptor.h"

void welcome(TCPStream *tcp_stream)
{
    char buf[100] = "~~~ welcome to epoll loop ~~~\n";
    tcp_stream->send(tcp_stream, buf, 100);
}

void handle(TCPStream *tcp_stream)
{
	char buf[100];
	memset(buf, 0, 100);
	sprintf(buf, "hi %d\n", tcp_stream->sockfd);
	tcp_stream->send(tcp_stream, buf, 100);

	if (tcp_stream->receive(tcp_stream, buf, 100) > 0) {
		fprintf(stderr, "%s\n", buf);
		if (strncmp(buf, "bye", 3) == 0) {
			strcpy(buf, "see you\n");
			tcp_stream->send(tcp_stream, buf, 100);
			close(tcp_stream->sockfd);
			tcp_stream->sockfd = 0;
		}
	}
}

int main(int argc, const char *argv[])
{
    TCPAcceptor *tcp_acceptor = NEW(TCPAcceptor, "ggg");
    tcp_acceptor->listen(tcp_acceptor);
    printf("listen for port=%s, listenfd=%d\n", tcp_acceptor->port, tcp_acceptor->listenfd);

    tcp_acceptor->epoll_loop(tcp_acceptor, welcome, handle);

    return 0;
}

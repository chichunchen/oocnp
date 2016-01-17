#ifndef __unp_h
#define __unp_h

#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>  /* inet(3) functions */
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "dbg.h"

#define TRUE        1
#define FALSE       0

/* shortens all the typecasts of pointer arguments: */
#define SA  struct sockaddr

#define LISTENQ             1024
#define SERV_PORT           "10123"

#define DEBUG               0

#endif

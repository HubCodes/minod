#ifndef _NET_H
#define _NET_H

#include <sys/socket.h>
#include <netinet/in.h>

typedef struct server {
	int sockfd;
	struct sockaddr_in addr;
	int tcp_backlog;
} server;

server* server_new(in_addr_t host, unsigned short port, int tcp_backlog);
void server_listen(server* self);

#endif

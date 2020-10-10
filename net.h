#ifndef _NET_H
#define _NET_H

#include "minod.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

enum { MAX_CLIENTS = FD_SETSIZE };

typedef struct connection {
	int clientfd;
	struct sockaddr_in clientaddr;
} connection;

typedef struct server {
	int sockfd;
	struct sockaddr_in addr;
	int tcp_backlog;
	connection* conn_list[MAX_CLIENTS];
} server;

server* server_new(in_addr_t host, unsigned short port, int tcp_backlog);
void server_listen(server* self, cmd_processor cb);

#endif

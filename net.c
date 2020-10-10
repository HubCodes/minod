#include "net.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>

static void server_listen_loop(server* self, callback cb);
static int recv_all(int clientfd, void* header, int header_size, int flags);

server* server_new(in_addr_t host, unsigned short port, int tcp_backlog) {
	int sockfd;
	struct sockaddr_in addr;
	server* self;

	self = calloc(sizeof(server), 1);
	assert(self != NULL);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	addr = (struct sockaddr_in){
		.sin_family=AF_INET,
		.sin_port=htons(port),
		.sin_addr=(struct in_addr){.s_addr=host}};

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		die("bind");
	}
	self->sockfd = sockfd;
	self->addr = addr;
	self->tcp_backlog = tcp_backlog;
	return self;
}

void server_listen(server* self, callback cb) {
	if (listen(self->sockfd, self->tcp_backlog) < 0) {
		die("listen");
	}
	server_listen_loop(self, cb);
}

static void server_listen_loop(server* self, callback cb) {
	int clientfd;
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len;
	connection* conn;
	fd_set all_fds, read_fds;

	FD_ZERO(&all_fds);
	FD_SET(self->sockfd, &all_fds);

	for (;;) {
		read_fds = all_fds;
		if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0) {
			die("select");
		}
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == self->sockfd) {
					clientfd = accept(
						self->sockfd,
						(struct sockaddr*)&clientaddr,
						&clientaddr_len);
					if (clientfd < 0) {
						die("accept");
					}
					conn = calloc(sizeof(connection), 1);
					conn->clientfd = clientfd;
					conn->clientaddr = clientaddr;
					self->conn_list[clientfd] = conn;
					conn = NULL;
					FD_SET(clientfd, &all_fds);
				} else {
					cb(self->conn_list[i]);
				}
			}
		}
	}
}

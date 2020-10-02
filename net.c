#include "net.h"

#include <assert.h>
#include <stdlib.h>

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
		perror("bind");
		exit(EXIT_FAILURE);
	}
	self->sockfd = sockfd;
	self->addr = addr;
	self->tcp_backlog = tcp_backlog;
	return self;
}

void server_listen(server* self) {
	if (listen(self->sockfd, self->tcp_backlog) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
}

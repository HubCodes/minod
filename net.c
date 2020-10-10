#include "net.h"
#include "util.h"

#include <assert.h>
#include <stdlib.h>

static void server_listen_loop(server* self, cmd_processor cb);
static void server_process_cmd(server* self, int clientfd, cmd_processor cb);
static int recv_all(int fd, char* buf, int header, int flags);

server* server_new(in_addr_t host, unsigned short port, int tcp_backlog) {
	int sockfd;
	struct sockaddr_in addr;
	server* self;

	self = calloc(sizeof(*self), 1);
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

void server_listen(server* self, cmd_processor cb) {
	if (listen(self->sockfd, self->tcp_backlog) < 0) {
		die("listen");
	}
	server_listen_loop(self, cb);
}

static void server_listen_loop(server* self, cmd_processor cb) {
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
					server_process_cmd(self, i, cb);
				}
			}
		}
	}
}

static void server_process_cmd(server* self, int clientfd, cmd_processor cb) {
	struct cmd_header cmd_header;
	struct cmd_body cmd_body;
	struct res_header res_header;
	struct res_body res_body;

	if (recv_all(clientfd, (void*)&cmd_header, sizeof(cmd_header), 0) < 0) {
		die("recv");
	}
	if (recv_all(
		clientfd,
		(void*)&cmd_body,
		sizeof(cmd_body) + cmd_header.size,
		0) < 0) {
		die("recv");
	}
	cb(&cmd_header, &cmd_body, &res_body, &res_header.size);
	if (send(clientfd, (void*)&res_header, sizeof(res_header), 0) < 0) {
		die("send");
	}
	if (send(
		clientfd,
		(void*)&res_body,
		sizeof(res_body) + res_header.size,
		0) < 0) {
		die("send");
	}
}

static int recv_all(int fd, char* buf, int size, int flags) {
    int ret;

    while (size != 0 && (ret = recv(fd, (void*)buf, size, flags)) != 0) {
        if (ret < 0) {
            return ret;
        }
        size -= ret;
        buf += ret;
    }

    return 0;	
}

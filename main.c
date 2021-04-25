#include "minod.h"
#include "hashmap.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

enum {
    LISTEN_PORT = 7070,
    TCP_BACKLOG = 2048,
};

static int recv_all(int fd, char* buf, int size, int flags);

int main(void) {
    int sockfd, clientfd;
    struct sockaddr clientaddr;
    socklen_t clientaddr_len;
    struct sockaddr_in addr;
    map* kv_store;

    kv_store = map_new();

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)LISTEN_PORT);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(sockfd, TCP_BACKLOG) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    for (;;) {
        struct cmd_header header;
        struct cmd* cmd;
        struct res* res;
        int valuesize, ressize, recv_errno;
        void* value;

        res = NULL;

        if ((clientfd = accept(sockfd, &clientaddr, &clientaddr_len)) < 0) {
            perror("accept");
            return EXIT_FAILURE;
        }

        if (clientfd > 0 && recv_all(clientfd, (void*)&header, sizeof(header), 0) < 0) {
            recv_errno = errno;
            if (recv_errno == ECONNRESET) {
                close(clientfd);
                clientfd = 0;
                continue;
            }
            fprintf(stderr, "recv: %s\n", strerror(recv_errno));
            free(res);
            close(clientfd);
            close(sockfd);
            return EXIT_FAILURE;
        }
        cmd = malloc(header.size);
        if (recv_all(clientfd, (char*)cmd + sizeof(header), header.size - sizeof(header), 0) < 0) {
            recv_errno = errno;
            if (errno == ECONNRESET) {
                close(clientfd);
                clientfd = 0;
                free(cmd);
                continue;
            }
            fprintf(stderr, "recv: %s\n", strerror(recv_errno));
            free(res);
            close(clientfd);
            close(sockfd);
            return EXIT_FAILURE;
        }
        cmd->header = header;
        if (cmd->header.type == CMD_GET) {
            value = map_get(kv_store, cmd->buf, &valuesize);
            if (value == NULL) {
                ressize = sizeof(*res);
                res = malloc(ressize);
                res->header.code = RES_NOT_FOUND;
                res->header.size = ressize;
                res->header.res_size = 0;
            } else {
                ressize = sizeof(*res) + valuesize;
                res = malloc(ressize);
                res->header.code = RES_SUCCESS;
                res->header.size = ressize;
                res->header.res_size = valuesize;
                memcpy(res->buf, value, valuesize);
            }
        } else if (cmd->header.type == CMD_SET) {
            char* set_buffer;

            set_buffer = malloc(cmd->key_size + cmd->value_size);
            memcpy(set_buffer, cmd->buf, cmd->key_size + cmd->value_size);
            map_set(kv_store, set_buffer, cmd->key_size, cmd->value_size);
            ressize = sizeof(*res);
            res = malloc(ressize);
            res->header.code = RES_SUCCESS;
            res->header.size = ressize;
            res->header.res_size = 0;
        } else {
            ressize = sizeof(*res);
            res = malloc(ressize);
            res->header.code = RES_FAILURE;
            res->header.size = ressize;
            res->header.res_size = 0;
        }
        send(clientfd, (void*)res, ressize, 0);
        free(res);
        free(cmd);
        close(clientfd);
        clientfd = 0;
        recv_errno = 0;
    }
    close(clientfd);
    close(sockfd);
    map_delete(kv_store);

    return EXIT_SUCCESS;
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


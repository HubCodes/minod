#include "hashmap.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

enum {
    IP_PROTO = 0,
    LISTEN_PORT = 7070,
    TCP_BACKLOG = 65535,
};

enum cmd_type {
    GET,
    SET,
};

struct cmd {
    enum cmd_type cmd_type;
    int key_size, value_size;
    char buf[];
};

void read_cmd(char* buf, struct cmd* cmd);

int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in addr;
    char buffer[256] = { 0, };

    sockfd = socket(PF_INET, SOCK_DGRAM, IP_PROTO);
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
        if (recv(sockfd, (void*)buffer, 256, 0) < 0) {
            perror("recv");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}


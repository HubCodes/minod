#include "minod.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// TODO: host와 port도 따로 받자
enum { SERVER_PORT = 7070 };

__attribute__ ((noreturn)) static void usage(const char* program);
static int parse_cmd(const int cmd_count, const char* cmd_list[], struct cmd** cmd);
static int strlen_trailing_null(const char* str);
static void print_res(struct res* res);

int main(const int argc, const char* argv[]) {
    struct cmd* cmd;
    struct res_header res_header;
    struct res* res;
    // TODO: ipv6 을 사용하자
    struct sockaddr_in addr;
    int serverfd;

    if (argc <= 1) {
        usage(argv[0]);
    }
    if (parse_cmd(argc - 1, argv + 1, &cmd) < 0) {
        free(cmd);
        usage(argv[0]);
    }

    serverfd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
        perror("socket");
        goto fail;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons((short)SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(serverfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        goto fail;
    }

    if (send(serverfd, (void*)cmd, cmd->header.size, 0) < 0) {
        perror("send");
        goto fail;
    }

    if (recv(serverfd, (void*)&res_header, sizeof(res_header), 0) < 0) {
        perror("recv");
        goto fail;
    }

    switch (res_header.code) {
        case RES_SUCCESS:
            res = malloc(res_header.size);
            if (recv(serverfd, (void*)&res->buf, res_header.res_size, 0) < 0) {
                perror("recv");
                goto fail;
            }
            res->header = res_header;
            print_res(res);
            free(res);
            break;
        case RES_FAILURE:
            fprintf(stderr, "minod: An error occurred while executing command.\n");
            goto fail;
        case RES_NOT_FOUND:
            fprintf(stderr, "minod: Key not found.\n");
            goto fail;
        default:
            fprintf(stderr, "minod: Unknown error code received from server.\n");
            goto fail;
    }

    free(cmd);
    close(serverfd);
    return EXIT_SUCCESS;
fail:
    free(cmd);
    close(serverfd);
    return EXIT_FAILURE;
}

__attribute__ ((noreturn)) static void usage(const char* program) {
    fprintf(stderr, "Usage: %s [get | set] [key, [value]]\n", program);
    exit(EXIT_FAILURE);
}

static int strlen_trailing_null(const char* str) {
    return strlen(str) + 1;
}

// TODO: 헤더만 파싱하는 함수를 따로 만들어서, 이 함수에서 malloc을 하지 않게끔 하자
static int parse_cmd(const int cmd_count, const char* cmd_list[], struct cmd** cmd) {
    const char* cmd_str, *key_str, *value_str;
    int key_size, value_size, cmd_size;

    cmd_str = cmd_list[0];
    if (strcmp(cmd_str, "get") == 0 && cmd_count == 2) {
        key_str = cmd_list[1];
        key_size = strlen_trailing_null(key_str);
        cmd_size = sizeof(**cmd) + key_size;

        *cmd = malloc(cmd_size);
        (*cmd)->header.type = CMD_GET;
        (*cmd)->header.size = cmd_size;
        (*cmd)->key_size = key_size;
        (*cmd)->value_size = 0;
        memcpy((*cmd)->buf, key_str, key_size);

        return 0;
    } else if (strcmp(cmd_str, "set") == 0 && cmd_count == 3) {
        key_str = cmd_list[1];
        value_str = cmd_list[2];
        key_size = strlen_trailing_null(key_str);
        value_size = strlen_trailing_null(value_str);
        cmd_size = sizeof(**cmd) + key_size + value_size;

        *cmd = malloc(cmd_size);
        (*cmd)->header.type = CMD_SET;
        (*cmd)->header.size = cmd_size;
        (*cmd)->key_size = key_size;
        (*cmd)->value_size = value_size;
        memcpy((*cmd)->buf, key_str, key_size);
        memcpy((*cmd)->buf + key_size, value_str, value_size);

        return 0;
    } 

    return -1;
}

static void print_res(struct res* res) {
    static const char* code_to_str[] = {
        NULL,
        "RES_FAILURE",
        "RES_SUCCESS",
        "RES_NOT_FOUND",
    };
    printf("struct res {\n");
    printf("  res_header: {\n");
    printf("    code: \"%s\",\n", code_to_str[res->header.code]);
    printf("    size: %d,\n", res->header.size);
    printf("    res_size: %d,\n", res->header.res_size);
    printf("  },\n");
    if (res->header.res_size == 0) {
        printf("}\n");
        return;
    }
    printf("  buf: \"%s\",\n", res->buf);
    printf("}\n");
}


#ifndef _MINOD_H
#define _MINOD_H

enum cmd_type {
    CMD_GET = 1,
    CMD_SET,
};

struct cmd_header {
    enum cmd_type type;
    int size;
};

struct cmd_body {
    int key_size, value_size;
    char buf[];
};

enum res_code {
    RES_FAILURE = 1,
    RES_SUCCESS,
    RES_NOT_FOUND,
};

struct res_header {
    int size;
};

struct res_body {
    enum res_code code;
    char buf[];
};

typedef void (*cmd_processor)(
    struct cmd_header* header,
    struct cmd_body* body,
    struct res_body* res,
    int* res_size);

#endif


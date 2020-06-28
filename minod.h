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

struct cmd {
    struct cmd_header header;
    int key_size, value_size;
    char buf[];
};

enum res_code {
    RES_FAILURE = 1,
    RES_SUCCESS,
    RES_NOT_FOUND,
};

struct res_header {
    enum res_code code;
    int size, res_size;
};

struct res {
    struct res_header header;
    char buf[];
};

#endif


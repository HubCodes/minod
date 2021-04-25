#ifndef _MINOD_H
#define _MINOD_H

#include <stdint.h>

#define CMD_GET 0x1
#define CMD_SET 0x2

#define RES_FAILURE 0x1
#define RES_SUCCESS 0x2
#define RES_NOT_FOUND 0x3

#pragma pack(push, 1)
typedef struct cmd {
    uint32_t size;
    uint8_t type;  // CMD_GET or CMD_SET
    uint32_t key_size, value_size;
    uint8_t buf[];
} cmd;

typedef struct res {
    uint32_t size;
    uint8_t code;
    uint8_t buf[];
} res;
#pragma pack(pop)

#endif


#include "minod.h"

#include <stddef.h>

void process_cmd(
    map* map,
    struct cmd_header* header,
    struct cmd_body* body,
    struct res_body* res,
    int* res_size) {
    char* result;

    switch (header->type) {
    case CMD_GET:
        result = map_get(map, body->buf, res_size);
        if (result == NULL) {
            res->code = RES_NOT_FOUND;
            *res_size = 0;
            return;
        }
        res->code = RES_SUCCESS;
        res->buf = result;
        return;
    }
}

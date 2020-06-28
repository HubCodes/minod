#ifndef _RAWMAP_H
#define _RAWMAP_H

typedef struct map_entry
{
    const char* buffer;
    int keysize, valuesize;
    struct map_entry* next;
} map_entry;
typedef struct map
{
    struct map_entry** bucket;
    int size;
    int entries;
} map;
map* map_new(void);
void map_set(map* self, const char* buffer, int keysize, int valuesize);
char* map_get(const map* self, const char* key, int* valuesize);
void map_remove(map* self, const char* key);
void map_delete(map* self);

#endif


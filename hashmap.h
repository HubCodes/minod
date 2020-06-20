#ifndef _HASHMAP_H
#define _HASHMAP_H

typedef struct MapEntry
{
    char* key;
    void* value;
    struct MapEntry* next;
} MapEntry;
typedef struct Map
{
    struct MapEntry** bucket;
    int size;
    int entries;
} Map;
Map* Map_new(void);
void Map_set(Map* self, char* key, void* value);
void* Map_get(Map* self, char* key);
void Map_remove(Map* self, char* key);

#endif


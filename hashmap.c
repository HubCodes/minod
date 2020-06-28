/*
 *  hashmap.c: rawmap implementation
 * 
 *  Separate chaining with linked list
 */

#include "rawmap.h"

#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 128

map* map_new(void) {
	map* self = calloc(sizeof(map), 1);
	self->bucket = calloc(sizeof(map_entry*), MAP_SIZE);
	self->size = MAP_SIZE;
	self->entries = 0;
	return self;
}

static unsigned int string_hash(const char* str) {
	unsigned int value = 37;
	while (*str)
	{
		value = (value * 54059) ^ (*str * 76963);
		str++;
	}
	return value;
}

static void map_rehash(map* self) {
	double load_factor = (double)self->entries / (double)self->size;
	if (load_factor < 0.75)
		return;
	int old_size = self->size;
	int new_size = old_size * 2;
	map_entry** old_bucket = self->bucket;
	map_entry** new_bucket = calloc(sizeof(map_entry*), new_size);
	self->bucket = new_bucket;
	self->entries = 0;
	self->size = new_size;
	for (int i = 0; i < old_size; i++)
	{
		map_entry* entry = old_bucket[i];
		map_entry* prev = NULL;
		while (entry)
		{
			map_set(self, entry->buffer, entry->keysize, entry->valuesize);
			prev = entry;
			entry = entry->next;
			free(prev);
		}
	}
	free(old_bucket);
}

static void map_delete_entry(map_entry* entry) {
    if (entry->next) {
        map_delete_entry(entry->next);
    }
    free(entry);
}

void map_set(map* self, const char* buffer, int keysize, int valuesize) {
	map_rehash(self);
	unsigned int key_index = string_hash(buffer) % self->size;
	map_entry* table_entry = self->bucket[key_index];
	map_entry* new_entry = calloc(sizeof(map_entry), 1);
	map_entry* prev_entry = NULL;
    new_entry->buffer = buffer;
    new_entry->keysize = keysize;
    new_entry->valuesize = valuesize;
	new_entry->next = NULL;
	if (!table_entry)
	{
		self->bucket[key_index] = new_entry;
		self->entries++;
		return;
	}
	while (table_entry)
	{
		int is_same_key = strcmp(buffer, table_entry->buffer) == 0;
		if (is_same_key && prev_entry)
		{
			new_entry->next = table_entry->next;
			prev_entry->next = new_entry;
			free(table_entry);
			return;
		}
		else if (is_same_key)
		{
			self->bucket[key_index] = new_entry;
			free(table_entry);
			return;
		}
		prev_entry = table_entry;
		table_entry = table_entry->next;
	}
	prev_entry->next = new_entry;
	self->entries++;
}

char* map_get(const map* self, const char* key, int* valuesize) {
	unsigned int key_index = string_hash(key) % self->size;
	map_entry* table_entry = self->bucket[key_index];
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->buffer) == 0;
		if (is_same_key) {
            *valuesize = table_entry->valuesize;
            return (char*)table_entry->buffer + table_entry->keysize;
        }
		table_entry = table_entry->next;
	}
	return NULL;
}

void map_remove(map* self, const char* key) {
	unsigned int key_index = string_hash(key) % self->size;
	map_entry* table_entry = self->bucket[key_index];
	map_entry* prev_entry = NULL;
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->buffer) == 0;
		if (is_same_key && prev_entry)
		{
			self->entries--;
			prev_entry->next = table_entry->next;
			free(table_entry);
			return;
		}
		else if (is_same_key)
		{
			self->entries--;
			self->bucket[key_index] = table_entry->next;
			free(table_entry);
			return;
		}
		prev_entry = table_entry;
		table_entry = table_entry->next;
	}
}

void map_delete(map* self) {
    int i;

    for (i = 0; i < self->entries; i++) {
        if (self->bucket[i]) {
            map_delete_entry(self->bucket[i]);
        }
    }
    free(self->bucket);
}

#undef MAP_SIZE


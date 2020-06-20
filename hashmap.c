/*
 *  Map: HashMap implementation
 * 
 *  Separate chaining with linked list
 */

#include "hashmap.h"

#include <stdlib.h>
#include <string.h>

#define MAP_SIZE 128

Map* Map_new(void)
{
	Map* self = calloc(sizeof(Map), 1);
	self->bucket = calloc(sizeof(MapEntry* ), MAP_SIZE);
	self->size = MAP_SIZE;
	self->entries = 0;
	return self;
}

static unsigned int string_hash(char* str)
{
	unsigned int value = 37;
	while (*str)
	{
		value = (value * 54059) ^ (*str * 76963);
		str++;
	}
	return value;
}

static void Map_rehash(Map* self)
{
	double load_factor = (double)self->entries / (double)self->size;
	if (load_factor < 0.75)
		return;
	int old_size = self->size;
	int new_size = old_size * 2;
	MapEntry** old_bucket = self->bucket;
	MapEntry** new_bucket = calloc(sizeof(MapEntry*), new_size);
	self->bucket = new_bucket;
	self->entries = 0;
	self->size = new_size;
	for (int i = 0; i < old_size; i++)
	{
		MapEntry* entry = old_bucket[i];
		MapEntry* prev = NULL;
		while (entry)
		{
			Map_set(self, entry->key, entry->value);
			prev = entry;
			entry = entry->next;
			free(prev);
		}
	}
	free(old_bucket);
}

void Map_set(Map* self, char* key, void* value)
{
	Map_rehash(self);
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry* table_entry = self->bucket[key_index];
	MapEntry* new_entry = calloc(sizeof(MapEntry), 1);
	MapEntry* prev_entry = NULL;
	new_entry->key = key;
	new_entry->value = value;
	new_entry->next = NULL;
	if (!table_entry)
	{
		self->bucket[key_index] = new_entry;
		self->entries++;
		return;
	}
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
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

void* Map_get(Map* self, char* key)
{
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry* table_entry = self->bucket[key_index];
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
		if (is_same_key)
			return table_entry->value;
		table_entry = table_entry->next;
	}
	return NULL;
}

void Map_remove(Map* self, char* key)
{
	unsigned int key_index = string_hash(key) % self->size;
	MapEntry* table_entry = self->bucket[key_index];
	MapEntry* prev_entry = NULL;
	while (table_entry)
	{
		int is_same_key = strcmp(key, table_entry->key) == 0;
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

#undef MAP_SIZE


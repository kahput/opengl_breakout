#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct _arena Arena;
typedef struct _hash_table HashTable;

HashTable *ht_create(Arena *arena, size_t type_size);

void ht_insert(HashTable *ht, const char *key, const void *value);
void *ht_search(HashTable *ht, const char *key);
void ht_remove(HashTable *ht, const char *key);

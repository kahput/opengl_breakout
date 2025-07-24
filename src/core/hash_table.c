#include "hash_table.h"

#include "core/arena.h"
#include "core/logger.h"

#include <math.h>
#include <string.h>

#define HT_PRIME_1 257
#define HT_PRIME_2 271

#define KEY_SIZE 256

struct _hash_table {
	size_t type_size, item_size;
	uint32_t count, capacity;
	void *items;
};

static int ht_hash(const char *s, const int a, const int m) {
	long hash = 0;
	const int len_s = strlen(s);
	for (int i = 0; i < len_s; i++) {
		hash += (long)pow(a, len_s - (i + 1)) * s[i];
		hash = hash % m;
	}
	return (int)hash;
}

static int ht_get_hash(
	const char *s, const int num_buckets, const int attempt) {
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

HashTable *ht_create(Arena *arena, size_t type_size) {
	HashTable *ht = arena_push_type(arena, HashTable);
	*ht = (HashTable){
		.capacity = 53,
		.count = 0,
		.items = NULL,
		.type_size = type_size,
	};

	ht->item_size = KEY_SIZE + ht->type_size;

	ht->items = arena_push_zero(arena, ht->item_size * ht->capacity);
	return ht;
}

void ht_insert(HashTable *ht, const char *key, const void *value) {
	int32_t index = 0;
	char *items = ht->items;
	for (int32_t i = 0;; i++) {
		index = ht_get_hash(key, ht->capacity, i);
		char *current_item = items + (ht->item_size * index);
		if (current_item[0] == '\0' || strncmp(current_item, key, KEY_SIZE) == 0)
			break;
	}

	LOG_INFO("Item being placed at index %i", index);
	memcpy(items + (ht->item_size * index), key, strlen(key) + 1);
	memcpy((items + (ht->item_size * index)) + KEY_SIZE, value, ht->type_size);
	ht->count++;
}
void *ht_search(HashTable *ht, const char *key) {
	int32_t index = 0;
	char *items = ht->items;
	for (int32_t i = 0;; i++) {
		index = ht_get_hash(key, ht->capacity, i);
		char *current_item = items + (ht->item_size * index);
		if (current_item[0] == '\0')
			return NULL;

		if (strncmp(current_item, key, KEY_SIZE) == 0) {
			return current_item + KEY_SIZE;
		}
	}
	return NULL;
}
void ht_remove(HashTable *ht, const char *key) {
	int32_t index = 0;
	char *items = ht->items;
	for (int32_t i = 0;; i++) {
		index = ht_get_hash(key, ht->capacity, i);
		char *current_item = items + (ht->item_size * index);
		if (current_item[0] == '\0')
			return;

		if (strncmp(current_item, key, KEY_SIZE) == 0) {
			// NOTE: Might be worth to set it all to 0 instead
			// memset(current_item, 0, ht->item_size);
			current_item[0] = '\0';
			ht->count--;
			return;
		}
	}
}

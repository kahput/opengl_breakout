#include "core/arena.h"
#include "core/hash_table.h"
#include "core/logger.h"
#include <stdio.h>
#include <string.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum {
	GAME_STATE_NONE,
	GAME_STATE_ACTIVE,
	GAME_STATE_MENU,
	GAME_STATE_WIN,

	GAME_STATE_COUNT
} GameState;

typedef struct {
	Arena *arena_permanent;
	GameState state;
	bool keys[1024];

	uint32_t width, height;
} Game;

void game_create(Game *game, uint32_t width, uint32_t height);

void game_process_input(Game *game);
void game_update(Game *game);
void game_draw(Game *game);

void test_hashtable(void);
void test_advanced_hashtable_features(void);

int main(void) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(640, 480, "Breakout", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);

	Game game = {
		.width = 640,
		.height = 480,
		.arena_permanent = arena_alloc(),
		.keys = NULL,
		.state = GAME_STATE_ACTIVE
	};

	test_hashtable();
	test_advanced_hashtable_features();

	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClearColor(255, 255, 255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void game_create(Game *game, uint32_t width, uint32_t height) {
}

void game_process_input(Game *game) {
}

void game_update(Game *game) {}

void game_draw(Game *game) {}

void test_hashtable(void) {
	LOG_INFO("== Running test_hashtable ==");

	Arena *arena = arena_alloc();
	assert(arena);
	LOG_DEBUG("Arena created");

	HashTable *ht = ht_create(arena, sizeof(int));
	assert(ht);
	LOG_DEBUG("Hashtable created with int type");

	int v1 = 42, v2 = 1337, v3 = 9001;

	LOG_INFO("Inserting three keys: alpha, beta, gamma");
	ht_insert(ht, "alpha", &v1);
	ht_insert(ht, "beta", &v2);
	ht_insert(ht, "gamma", &v3);

	LOG_INFO("Searching inserted keys");
	int *found = ht_search(ht, "alpha");
	assert(found && *found == 42);
	LOG_DEBUG("Found alpha: %d", *found);

	found = ht_search(ht, "beta");
	assert(found && *found == 1337);
	LOG_DEBUG("Found beta: %d", *found);

	found = ht_search(ht, "gamma");
	assert(found && *found == 9001);
	LOG_DEBUG("Found gamma: %d", *found);

	LOG_INFO("Searching non-existent key delta");
	found = ht_search(ht, "delta");
	assert(found == NULL);
	LOG_DEBUG("delta correctly not found");

	LOG_INFO("Removing key beta");
	ht_remove(ht, "beta");
	found = ht_search(ht, "beta");
	assert(found == NULL);
	LOG_DEBUG("beta correctly removed");

	LOG_INFO("Re-inserting beta with new value");
	int new_beta = 555;
	ht_insert(ht, "beta", &new_beta);
	found = ht_search(ht, "beta");
	assert(found && *found == 555);
	LOG_DEBUG("beta now has new value: %d", *found);

	LOG_INFO("Updating existing key gamma to new value");
	int new_gamma = 777;
	ht_insert(ht, "gamma", &new_gamma);
	found = ht_search(ht, "gamma");
	assert(found && *found == 777);
	LOG_DEBUG("gamma updated: %d", *found);

	LOG_INFO("Inserting many keys to check collisions and robustness");
	char key[32];
	const int N = 100;
	for (int i = 0; i < N; i++) {
		snprintf(key, sizeof(key), "key_%d", i);
		ht_insert(ht, key, &i);
	}

	for (int i = 0; i < N; i++) {
		snprintf(key, sizeof(key), "key_%d", i);
		found = ht_search(ht, key);
		assert(found && *found == i);
	}
	LOG_DEBUG("All %d keys correctly inserted and found", N);

	LOG_INFO("Removing half of the keys");
	for (int i = 0; i < N; i += 2) {
		snprintf(key, sizeof(key), "key_%d", i);
		ht_remove(ht, key);
	}
	for (int i = 0; i < N; i++) {
		snprintf(key, sizeof(key), "key_%d", i);
		found = ht_search(ht, key);
		if (i % 2 == 0) {
			assert(found == NULL);
		} else {
			assert(found && *found == i);
		}
	}
	LOG_DEBUG("Removal of half the keys verified");

	LOG_INFO("Testing wrong usage: inserting NULL key (should not crash)");
	ht_insert(ht, NULL, &v1);
	found = ht_search(ht, NULL);
	if (found) {
		LOG_WARN("Unexpectedly found value for NULL key: %d", *found);
	} else {
		LOG_DEBUG("NULL key correctly not found or ignored");
	}

	LOG_INFO("Testing wrong usage: inserting with wrong type size (simulate by casting float*)");
	float fv = 3.14f;
	ht_insert(ht, "floatkey", &fv);
	int *wrong_found = ht_search(ht, "floatkey");
	if (wrong_found) {
		LOG_WARN("Inserted float, got int bits: %d (expected, but programmer error)", *wrong_found);
	} else {
		LOG_DEBUG("floatkey correctly not found or rejected");
	}

	LOG_INFO("Testing edge case: very long key");
	char longkey[1024];
	memset(longkey, 'A', sizeof(longkey) - 1);
	longkey[sizeof(longkey) - 1] = '\0';
	ht_insert(ht, longkey, &v1);
	found = ht_search(ht, longkey);
	assert(found && *found == 42);
	LOG_DEBUG("Long key inserted and found");

	arena_free(arena);
	LOG_INFO("test_hashtable finished successfully.\n");
}
void test_advanced_hashtable_features(void) {
	LOG_INFO("== Running test_advanced_hashtable_features ==");

	Arena *arena = arena_alloc();
	// For this test, let's assume TABLE_CAPACITY is at least 10.
	// If not, these values should be adjusted.
	HashTable *ht = ht_create(arena, sizeof(int));
	assert(ht);

	// === 1. Test Tombstone Deletion and Probe Chains ===
	LOG_INFO("Testing tombstone functionality...");
	int val_a = 10, val_b = 20, val_c = 30;
	// These keys are chosen to likely cause collisions on simple hash functions.
	ht_insert(ht, "key_11", &val_a);
	ht_insert(ht, "key_22", &val_b);
	ht_insert(ht, "key_33", &val_c);

	// Remove the middle element of a potential probe chain.
	ht_remove(ht, "key_22");
	LOG_DEBUG("Removed 'key_22'.");

	// Search for the element AFTER the tombstone.
	// If tombstones aren't handled, this search will fail.
	int *found_c = ht_search(ht, "key_33");
	assert(found_c && *found_c == val_c);
	LOG_DEBUG("Successfully found 'key_33' after deleting a colliding key.");

	// Ensure the removed element is truly gone.
	assert(ht_search(ht, "key_22") == NULL);
	LOG_DEBUG("'key_22' is correctly not found.");

	// Ensure we can insert into the tombstone slot.
	int val_d = 40;
	ht_insert(ht, "key_44", &val_d);
	int *found_d = ht_search(ht, "key_44");
	assert(found_d && *found_d == val_d);
	LOG_DEBUG("Successfully inserted 'key_44' into a tombstone slot.");

	// === 2. Test `count` is not incremented on value update ===
	LOG_INFO("Testing count on update...");
	uint32_t count_before_update = ht_length(ht);
	int new_val_a = 100;

	ht_insert(ht, "key_11", &new_val_a); // Update existing key
	uint32_t count_after_update = ht_length(ht);

	assert(count_before_update == count_after_update);
	LOG_DEBUG("Count did not change after update (Before: %u, After: %u).", count_before_update, count_after_update);

	int *found_updated_a = ht_search(ht, "key_11");
	assert(found_updated_a && *found_updated_a == new_val_a);
	LOG_DEBUG("Value for 'key_11' was correctly updated to %d.", *found_updated_a);

	// === 3. Test for correct key matching (no prefix matching) ===
	LOG_INFO("Testing for exact key matching (no prefix bugs)...");
	int val_prefix = 500, val_prefix_long = 600;
	ht_insert(ht, "prefix", &val_prefix);
	ht_insert(ht, "prefix_long", &val_prefix_long);

	int *found_prefix = ht_search(ht, "prefix");
	assert(found_prefix && *found_prefix == val_prefix);
	LOG_DEBUG("Searching for 'prefix' correctly returned its value (%d), not the longer key's value.", *found_prefix);

	// === 4. Test table-full behavior (won't hang on infinite loop) ===
	LOG_INFO("Testing behavior when table is full...");
	// Assuming TABLE_CAPACITY is 53, fill it up.
	// We already have a few keys, so subtract them from the loop count.
	uint32_t fill_count = HT_CAPACITY - ht_length(ht);
	LOG_INFO("The hashtable count is %i, difference is %i", ht_length(ht), fill_count);
	char key_buffer[32];
	for (int i = 0; i < fill_count; i++) {
		snprintf(key_buffer, sizeof(key_buffer), "fill_key_%d", i);
		ht_insert(ht, key_buffer, &i);
	}
	LOG_INFO("The hashtable count is %i", ht_length(ht));
	assert(ht_length(ht) == HT_CAPACITY);
	LOG_DEBUG("Table is now full with %u items.", ht_length(ht));

	// This call should return gracefully, not hang.
	int extra_val = 999;
	ht_insert(ht, "extra_key", &extra_val);
	LOG_DEBUG("Attempted to insert into a full table.");

	// The extra key should not have been inserted.
	assert(ht_search(ht, "extra_key") == NULL);
	LOG_DEBUG("'extra_key' was not inserted into the full table, as expected.");
	assert(ht_length(ht) == HT_CAPACITY); // Count should not have changed.

	arena_free(arena);
	LOG_INFO("test_advanced_hashtable_features finished successfully.\n");
}

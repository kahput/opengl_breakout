#include "core/arena.h"
#include "core/hash_table.h"
#include "core/logger.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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
	HashTable *ht = ht_create(game.arena_permanent, sizeof(int32_t));
	int32_t meaning = 32;
	ht_insert(ht, "Hi", &meaning);
	meaning = 42;
	ht_insert(ht, "Hi", &meaning);
	int32_t *saved_meaning = (int32_t *)ht_search(ht, "Hi");
	meaning = *saved_meaning;
	LOG_INFO("The meaning is %i", meaning);
	ht_remove(ht, "Hi");
	void *rv = ht_search(ht, "Hi");
	LOG_INFO("value_deleted=%b", rv ? false : true);

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

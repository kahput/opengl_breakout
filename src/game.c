#include "game.h"

#include "core/arena.h"

#include "asset_manager.h"

#include <glad/gl.h>
typedef enum {
	GAME_STATE_NONE,
	GAME_STATE_ACTIVE,
	GAME_STATE_MENU,
	GAME_STATE_WIN,

	GAME_STATE_COUNT
} GameState;

static Arena *arena_permanent;
struct _game {
	GameState state;
	bool keys[1024];

	uint32_t quad_vao;

	uint32_t width, height;
};

Game *game_create(uint32_t width, uint32_t height) {
	arena_permanent = arena_alloc();
	Game *game = arena_push_type(arena_permanent, Game);
	*game = (Game){
		.width = width,
		.height = height,
		.keys = NULL,
		.state = GAME_STATE_ACTIVE
	};

	asset_manager_startup();
	asset_manager_load_shader("default_shader", "assets/shaders/v_default.glsl", "assets/shaders/f_default.glsl");
	asset_manager_load_texture("sprite", "./assets/sprites/player.png");

	// clang-format off
	float vertices[] = {
		-0.5f,  0.5f, 0.0f, 1.0f,   // top left
		 0.5f,  0.5f, 1.0f, 1.0f,   // top right
		-0.5f, -0.5f, 0.0f, 0.0f,   // bottom left
		 0.5f, -0.5f, 1.0f, 0.0f    // bottom right
	};
	unsigned int indices[] = {
		// note that we start from 0!
		0, 2, 1, // first triangle
		1, 2, 3 // second triangle
	};
	// clang-format on

	uint32_t vbo, ibo;

	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

	glCreateBuffers(1, &ibo);
	glNamedBufferStorage(ibo, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &game->quad_vao);

	glVertexArrayVertexBuffer(game->quad_vao, 0, vbo, 0, 4 * sizeof(*vertices));
	glVertexArrayElementBuffer(game->quad_vao, ibo);

	glEnableVertexArrayAttrib(game->quad_vao, 0);
	glEnableVertexArrayAttrib(game->quad_vao, 1);

	glVertexArrayAttribFormat(game->quad_vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(game->quad_vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(*vertices));
	glVertexArrayAttribBinding(game->quad_vao, 0, 0);
	glVertexArrayAttribBinding(game->quad_vao, 1, 0);

	return game;
}

void game_process_input(Game *game) {
}

void game_update(Game *game) {}

void game_draw(Game *game) {
	opengl_shader_activate(asset_manager_get_shader("default_shader"));

	opengl_texture_activate(asset_manager_get_texture("sprite"), 0);
	opengl_shader_seti(asset_manager_get_shader("default_shader"), "u_texture", 0);

	glBindVertexArray(game->quad_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

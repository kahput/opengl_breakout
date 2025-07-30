#include "game.h"

#include "core/arena.h"
#include "core/logger.h"

#include "asset_manager.h"
#include "renderer.h"
#include "shader.h"

#include <cglm/cglm.h>
#include <ctype.h>
#include <glad/gl.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 512

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

	uint32_t width, height;
	Level *level;

	Renderer *renderer;
};

Game *game_create(uint32_t width, uint32_t height) {
	arena_permanent = arena_alloc();
	Game *game = arena_push_type(arena_permanent, Game);
	*game = (Game){
		.width = width,
		.height = height,
		.keys = { 0 },
		.state = GAME_STATE_ACTIVE
	};

	asset_manager_startup();
	OpenGLShader *shader = asset_manager_load_shader("default", "assets/shaders/v_default.glsl", "assets/shaders/f_default.glsl");
	asset_manager_load_texture("sprite", "./assets/sprites/player.png");

	mat4 projection;
	glm_mat4_identity(projection);
	glm_ortho(0.0f, game->width, game->height, 0.0f, -1.0f, 1.0f, projection);

	opengl_shader_activate(shader);
	opengl_shader_seti(shader, "u_texture", 0);
	opengl_shader_set4fm(shader, "u_projection", *projection);

	game->renderer = renderer_create(arena_permanent, shader);
	game->level = game_load_level("./assets/levels/level_01.csv", game->width, game->height);

	return game;
}

void game_process_input(Game *game) {
	(void)game;
}

void game_update(Game *game) {
	(void)game;
}

void game_draw(Game *game) {
	for (uint32_t i = 0; i < game->level->count; i++) {
		Sprite sprite = game->level->bricks[i];
		renderer_draw_sprite(game->renderer, sprite.texture, sprite.position, sprite.size, sprite.rotation, sprite.color);
	}
	renderer_draw_sprite(game->renderer, asset_manager_get_texture("sprite"), (vec2){ 100.0f, 100.0f }, (vec2){ 100.0f, 100.0f }, 0.0f, (vec3){ 1.0f, 1.0f, 1.0f });
}

Level *game_load_level(const char *path, uint32_t level_width, uint32_t level_height) {
	Level *level = arena_push_type(arena_permanent, Level);

	FILE *file;
	if ((file = fopen(path, "r")) == NULL) {
		LOG_ERROR("FILE: %s", strerror(errno));
		return NULL;
	}

	char buffer[MAX_LINE_LENGTH];
	uint32_t max_file_line = 0, max_file_column = 0;

	for (; fgets(buffer, sizeof(buffer), file); max_file_line++) {
		char *token = strtok(buffer, " \t\r\n");

		for (uint32_t x = 0; token; x++) {
			max_file_column = x == max_file_column ? x + 1 : max_file_column;

			LOG_INFO("Token [%d, %d]: %c", x, max_file_column, *token);
			token = strtok(NULL, " \t\r\n");
		}
	}

	rewind(file);

	level->capacity = max_file_line * max_file_column;
	level->count = 0;
	level->bricks = arena_push_array(arena_permanent, Sprite, level->capacity);

	for (uint32_t y = 0; fgets(buffer, sizeof(buffer), file); y++) {
		char *token = strtok(buffer, " \t\r\n");

		for (uint32_t x = 0; x < max_file_column; x++) {
			uint32_t index = x + y * max_file_column;
			if (token == NULL)
				LOG_WARN("LEVEL: Token [%d, %d] missing", x, y);

			else if (isdigit(*token) == 0)
				LOG_WARN("LEVEL: Token [%d, %d] missing", x, y);

			OpenGLTexture *texture = asset_manager_get_texture("sprite");
			int grid_size = ((level_width / max_file_column) / 16) * 16;

			level->bricks[level->count++] = (Sprite){
				.texture = texture,
				.color = { 1.0f, 1.0f, 1.0f },
				.position = { x * grid_size, y * grid_size },
				.size = { grid_size, grid_size },
				.rotation = 0.0f,

			};

			token = strtok(NULL, " \t\r\n");
		}
	}

	fclose(file);

	return level;
}

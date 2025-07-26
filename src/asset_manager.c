#include "asset_manager.h"

#include "core/arena.h"
#include "core/hash_table.h"
#include "core/logger.h"

#include "shader.h"
#include "texture.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <stb/stb_image.h>

typedef struct {
	Arena *asset_arena;
	HashTable *textures, *shaders;
} AssetManager;

static AssetManager g_asset_manager = { 0 };

void asset_manager_startup() {
	g_asset_manager.asset_arena = arena_alloc();
	g_asset_manager.shaders = ht_create(g_asset_manager.asset_arena, sizeof(size_t));
	g_asset_manager.textures = ht_create(g_asset_manager.asset_arena, sizeof(size_t));
}
void asset_manager_shutdown() {
	arena_free(g_asset_manager.asset_arena);
}

OpenGLShader *asset_manager_load_shader(const char *name, const char *vertex_shader_path, const char *fragment_shader_path) {
	FILE *file = fopen(vertex_shader_path, "r");

	if (fseek(file, 0, SEEK_END) == -1)
		LOG_ERROR("FILE: %s", strerror(errno));
	uint32_t offset = ftell(file);
	rewind(file);

	char vertex_shader_source[offset + 1];

	fread(vertex_shader_source, 1, offset, file);
	fclose(file);

	vertex_shader_source[offset] = '\0';

	file = fopen(fragment_shader_path, "r");

	if (fseek(file, 0, SEEK_END) == -1)
		LOG_ERROR("FILE: %s", strerror(errno));
	offset = ftell(file);
	rewind(file);

	char fragment_shader_source[offset + 1];

	fread(fragment_shader_source, 1, offset, file);
	fclose(file);

	fragment_shader_source[offset] = '\0';

	OpenGLShader *new_shader = opengl_shader_create(g_asset_manager.asset_arena, vertex_shader_source, fragment_shader_source);
	ht_insert(g_asset_manager.shaders, name, &new_shader);
	return new_shader;
}

OpenGLShader *asset_manager_get_shader(const char *name) {
	return *((OpenGLShader **)ht_search(g_asset_manager.shaders, name));
}

OpenGLTexture *asset_manager_load_texture(const char *name, const char *path) {
	stbi_set_flip_vertically_on_load(true);

	int32_t width, height, channel_count;
	uint8_t *data = stbi_load(path, &width, &height, &channel_count, 0);
	if (!data) {
		LOG_ERROR("Texture path [ %s ] not found", path);
		exit(1);
	}

	OpenGLTexture *new_texture = opengl_texture_load(g_asset_manager.asset_arena, width, height, channel_count, data);
	stbi_image_free(data);

	ht_insert(g_asset_manager.textures, name, &new_texture);
	return new_texture;
}

OpenGLTexture *asset_manager_get_texture(const char *name) {
	return *((OpenGLTexture **)ht_search(g_asset_manager.textures, name));
}

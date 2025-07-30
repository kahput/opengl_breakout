#include "texture.h"

#include "core/arena.h"
#include "core/logger.h"

#include <glad/gl.h>
#include <stb/stb_image.h>


OpenGLTexture *opengl_texture_load(Arena *arena, uint32_t width, uint32_t height, uint32_t channels, const uint8_t *pixels) {
	OpenGLTexture *texture = arena_push_type(arena, OpenGLTexture);

	glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);

	glTextureParameteri(texture->id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture->id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(texture->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureStorage2D(texture->id, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(texture->id, 0, 0, 0, width, height, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	texture->width = width;
	texture->height = height;
	texture->channels = channels;
	texture->path = NULL;
	return texture;
}
void opengl_texture_destroy(OpenGLTexture *texture) {
	if (texture) {
		OpenGLTexture *gl_texture = (OpenGLTexture *)texture;
		glDeleteTextures(1, &gl_texture->id);
		free(texture);
	}
}

void opengl_texture_activate(OpenGLTexture *texture, uint32_t texture_unit) {
	if (texture == NULL) {
		LOG_ERROR("Null texture passed to texture_activate!");
		exit(1);
	}
	OpenGLTexture *gl_texture = (OpenGLTexture *)texture;
	glBindTextureUnit(texture_unit, gl_texture->id);
}

#pragma once

#include <stdint.h>

typedef struct _arena Arena;
typedef struct _gl_texture OpenGLTexture;

OpenGLTexture *opengl_texture_load(Arena *arena, uint32_t width, uint32_t height, uint32_t channels, const uint8_t *pixels);
void opengl_texture_destroy(OpenGLTexture *texture);

void opengl_texture_activate(OpenGLTexture *texture, uint32_t texture_unit);

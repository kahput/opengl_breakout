#pragma once

#include <stdint.h>

typedef struct _arena Arena;
typedef struct _gl_texture OpenGLTexture;

OpenGLTexture* opengl_texture_load(Arena* arena, const char* texture_path);
void opengl_texture_destroy(OpenGLTexture* texture);

void opengl_texture_activate(OpenGLTexture* texture, uint32_t texture_unit);

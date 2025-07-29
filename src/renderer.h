#pragma once

#include <cglm/cglm.h>

typedef struct _arena Arena;

typedef struct _gl_texture OpenGLTexture;
typedef struct _gl_shader OpenGLShader;

typedef struct _renderer Renderer;

Renderer *renderer_create(Arena *arena, OpenGLShader *shader);

void renderer_draw_sprite(Renderer* renderer, OpenGLTexture *texture, vec2 position, vec2 size, float rotate, vec3 color);

#pragma once

#include <stdint.h>

typedef struct _arena Arena;
typedef struct _gl_shader OpenGLShader;

OpenGLShader *opengl_shader_create(Arena *arena, const char *vertex_shader_source, const char *fragment_shader_source);

void opengl_shader_activate(const OpenGLShader *shader);
void opengl_shader_deactivate(const OpenGLShader *shader);

void opengl_shader_seti(OpenGLShader *shader, const char *name, int32_t value);
void opengl_shader_setf(OpenGLShader *shader, const char *name, float value);
void opengl_shader_set2fv(OpenGLShader *shader, const char *name, float *value);
void opengl_shader_set3fv(OpenGLShader *shader, const char *name, float *value);
void opengl_shader_set4fv(OpenGLShader *shader, const char *name, float *value);
void opengl_shader_set4fm(OpenGLShader *shader, const char *name, float *value);

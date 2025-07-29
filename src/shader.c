#include "shader.h"

#include "core/arena.h"
#include "core/logger.h"

#include <glad/gl.h>
#include <string.h>

struct _gl_shader {
	uint32_t program;
};

OpenGLShader *opengl_shader_create(Arena *arena, const char *vertex_shader_source, const char *fragment_shader_source) {
	uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);

	uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);

	uint32_t program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	OpenGLShader *shader = arena_push_type(arena, OpenGLShader);
	*shader = (OpenGLShader){
		.program = program
	};

	return shader;
}

void opengl_shader_activate(const OpenGLShader *shader) {
	glUseProgram(shader->program);
}
void opengl_shader_deactivate(const OpenGLShader *shader) {
	(void)shader;
	glUseProgram(0);
}

void opengl_shader_seti(OpenGLShader *shader, const char *name, int32_t value) {
	glUniform1i(glGetUniformLocation(shader->program, name), value);
}
void opengl_shader_setf(OpenGLShader *shader, const char *name, float value) {
	glUniform1f(glGetUniformLocation(shader->program, name), value);
}
void opengl_shader_set2fv(OpenGLShader *shader, const char *name, float *value) {
	glUniform2fv(glGetUniformLocation(shader->program, name), 1, value);
}
void opengl_shader_set3fv(OpenGLShader *shader, const char *name, float *value) {
	glUniform3fv(glGetUniformLocation(shader->program, name), 1, value);
}
void opengl_shader_set4fv(OpenGLShader *shader, const char *name, float *value) {
	glUniform4fv(glGetUniformLocation(shader->program, name), 1, value);
}
void opengl_shader_set4fm(OpenGLShader *shader, const char *name, float *value) {
	glUniformMatrix4fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, value);
}

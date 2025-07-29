#include "renderer.h"

#include "core/arena.h"
#include "shader.h"
#include "texture.h"

#include <cglm/affine-pre.h>
#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <glad/gl.h>

#include <stdint.h>

struct _renderer {
	uint32_t quad_vao;
	OpenGLShader *shader;
};

Renderer *renderer_create(Arena *arena, OpenGLShader *shader) {
	Renderer *renderer = arena_push_type(arena, Renderer);
	renderer->shader = shader;

	// clang-format off
	float vertices[] = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
	// unsigned int indices[] = {
	// 	// note that we start from 0!
	// 	0, 2, 1, // first triangle
	// 	1, 2, 3 // second triangle
	// };
	// clang-format on

	uint32_t vbo, ibo;

	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, sizeof(vertices), vertices, GL_DYNAMIC_STORAGE_BIT);

	// glCreateBuffers(1, &ibo);
	// glNamedBufferStorage(ibo, sizeof(indices), indices, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &renderer->quad_vao);

	glVertexArrayVertexBuffer(renderer->quad_vao, 0, vbo, 0, 4 * sizeof(*vertices));
	// glVertexArrayElementBuffer(renderer->quad_vao, ibo);

	glEnableVertexArrayAttrib(renderer->quad_vao, 0);
	// glEnableVertexArrayAttrib(renderer->quad_vao, 1);

	glVertexArrayAttribFormat(renderer->quad_vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
	// glVertexArrayAttribFormat(renderer->quad_vao, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(*vertices));
	glVertexArrayAttribBinding(renderer->quad_vao, 0, 0);
	// glVertexArrayAttribBinding(renderer->quad_vao, 1, 0);

	return renderer;
}

void renderer_draw_sprite(Renderer *renderer, OpenGLTexture *texture, vec2 position, vec2 size, float rotate, vec3 color) {
	opengl_shader_activate(renderer->shader);

	mat4 model;
	glm_mat4_identity(model);
	glm_translate(model, (float[3]){ position[0], position[1], 0.0f });

	glm_translate(model, (vec3){ size[0] * 0.5f, size[1] * 0.5f, 0.0f });
	glm_rotate(model, glm_rad(rotate), (vec3){ 0.0f, 0.0f, 1.0f });
	glm_translate(model, (vec3){ size[0] * -0.5f, size[1] * -0.5f, 0.0f });

	glm_scale(model, (vec3){ size[0], size[1], 0.0f });

	opengl_shader_set4fm(renderer->shader, "u_model", *model);
	opengl_shader_set3fv(renderer->shader, "u_color", color);

	opengl_texture_activate(texture, 0);

	glBindVertexArray(renderer->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

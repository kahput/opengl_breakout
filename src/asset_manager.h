#pragma once

#include "shader.h"
#include "texture.h"

void asset_manager_startup();
void asset_manager_shutdown();

OpenGLShader *asset_manager_load_shader(const char *name, const char *vertex_shader_path, const char *fragment_shader_path);
OpenGLShader *asset_manager_get_shader(const char *name);

OpenGLTexture *asset_manager_load_texture(const char *name, const char *path);
OpenGLTexture *asset_manager_get_texture(const char *name);

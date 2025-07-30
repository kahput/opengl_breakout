#include "core/arena.h"
#include "core/hash_table.h"
#include "core/logger.h"

#include "asset_manager.h"
#include "game.h"
#include "shader.h"
#include "texture.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

const uint32_t SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;

typedef struct _display {
	GLFWwindow *window;
	uint32_t width, height;
} Display;

void initialize_display(Display *display);
void gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param);


int main(void) {
	Display display = {
		.width = SCREEN_WIDTH,
		.height = SCREEN_HEIGHT
	};
	initialize_display(&display);
	Game *game = game_create(SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(display.window)) {
		int width, height;
		glfwGetFramebufferSize(display.window, &width, &height);

		game_process_input(game);
		game_update(game);

		glViewport(0, 0, width, height);
		glClearColor(255, 255, 255, 255);
		glClear(GL_COLOR_BUFFER_BIT);

		game_draw(game);

		glfwSwapBuffers(display.window);
		glfwPollEvents();
	}

	glfwDestroyWindow(display.window);

	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void gl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei size, GLchar const *message, void const *user_param) {
	(void)user_param, (void)size;

	char src_str[32];

	switch (source) {
		case GL_DEBUG_SOURCE_API: {
			snprintf(src_str, sizeof(src_str), "API");
		} break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
			snprintf(src_str, sizeof(src_str), "WINDOW SYSTEM");
		} break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {
			snprintf(src_str, sizeof(src_str), "SHADER COMPILER");
		} break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: {
			snprintf(src_str, sizeof(src_str), "THIRD PARTY");
		} break;
		case GL_DEBUG_SOURCE_APPLICATION: {
			snprintf(src_str, sizeof(src_str), "APPLICATION");
		} break;
		case GL_DEBUG_SOURCE_OTHER: {
			snprintf(src_str, sizeof(src_str), "OTHER");
		} break;
	}

	char type_str[32];

	switch (type) {
		case GL_DEBUG_TYPE_ERROR: {
			snprintf(type_str, sizeof(type_str), "ERROR");
		} break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
			snprintf(type_str, sizeof(type_str), "DEPRECATED_BEHAVIOR");
		} break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
			snprintf(type_str, sizeof(type_str), "UNDEFINED_BEHAVIOR");
		} break;
		case GL_DEBUG_TYPE_PORTABILITY: {
			snprintf(type_str, sizeof(type_str), "PORTABILITY");
		} break;
		case GL_DEBUG_TYPE_PERFORMANCE: {
			snprintf(type_str, sizeof(type_str), "PERFORMANCE");
		} break;
		case GL_DEBUG_TYPE_MARKER: {
			snprintf(type_str, sizeof(type_str), "MARKER");
		} break;
		case GL_DEBUG_TYPE_OTHER: {
			snprintf(type_str, sizeof(type_str), "OTHER");
		} break;
	}

	char severity_str[32];

	switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: {
			snprintf(severity_str, sizeof(severity_str), "NOTIFICATION");
			LOG_TRACE("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
		} break;
		case GL_DEBUG_SEVERITY_LOW: {
			snprintf(severity_str, sizeof(severity_str), "LOW");
			LOG_INFO("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
		} break;
		case GL_DEBUG_SEVERITY_MEDIUM: {
			snprintf(severity_str, sizeof(severity_str), "MEDIUM");
			LOG_WARN("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
		} break;
		case GL_DEBUG_SEVERITY_HIGH: {
			snprintf(severity_str, sizeof(severity_str), "HIGH");
			LOG_ERROR("%s, %s, %s, %d: %s\n", src_str, type_str, severity_str, id, message);
		} break;
	}
}

void initialize_display(Display *display) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	display->window = glfwCreateWindow(display->width, display->height, "Breakout", NULL, NULL);
	if (!display->window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(display->window);
	gladLoadGL(glfwGetProcAddress);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(gl_message_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
}

#include "texture.h"

#include <cglm/cglm.h>

typedef struct sprite {
	vec2 position, size;
	float rotation;

	OpenGLTexture *texture;
	vec3 color;

	bool is_solid, is_destroyed;
	uint16_t _padding;
} Sprite;

typedef struct level {
	uint32_t count, capacity;
	Sprite *bricks;
} Level;

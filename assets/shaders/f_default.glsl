#version 450 core

out vec4 fragment_color;

in vec2 texture_coordinate;

uniform sampler2D u_texture;
uniform vec3 u_color;

void main() {
    fragment_color = vec4(u_color, 1.0f) * texture(u_texture, texture_coordinate);
};

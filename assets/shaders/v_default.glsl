#version 450 core

layout(location = 0) in vec4 vertex;

out vec2 texture_coordinate;

uniform mat4 u_model;
uniform mat4 u_projection;

void main() {
    gl_Position = u_projection * u_model * vec4(vertex.xy, 0.0f, 1.0f);
    texture_coordinate = vertex.zw;
}

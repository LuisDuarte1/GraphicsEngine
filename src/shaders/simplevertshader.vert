#version 450

layout(binding = 0) uniform UniformBufferObject{
    mat4 mvp;
} ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv_coords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;



void main() {
    gl_Position = ubo.mvp * vec4(position, 1.0);
    fragColor = color;
    fragTexCoord = uv_coords;
}

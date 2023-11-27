#version 450 core

layout(location = 0) in vec3 vPos;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;

out vec3 FragPos;

void main(void) {
    FragPos = vPos;
    vec3 pos = vPos + camPos;
    gl_Position = projection * view * vec4(pos,1.0);
}
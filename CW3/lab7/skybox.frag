#version 330 core

uniform samplerCube skyTexture;

in vec3 FragPos;

void main(void) {
    gl_FragColor = texture(skyTexture, normalize(FragPos));
}
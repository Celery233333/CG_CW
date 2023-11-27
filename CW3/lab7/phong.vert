#version 450 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNor;
layout(location = 2) in vec2 vUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projectedLightSpaceMatrix;


out vec2 uv;
out vec3 nor;
out vec3 FragPosWorldSpace;
out vec4 FragPosProjectedLightSpace;

void main()
{
	vec4 pos = vec4(vPos,1.0);
	gl_Position = projection * view * model * pos;
	uv = vUV;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * pos);
	FragPosProjectedLightSpace = projectedLightSpaceMatrix * model * pos;
}

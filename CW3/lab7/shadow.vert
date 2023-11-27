#version 450 core
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNor;
layout(location = 2) in vec2 vUV;

uniform mat4 projectedLightSpaceMatrix; 
uniform mat4 model;
void main()
{
	gl_Position = projectedLightSpaceMatrix * model * vec4(vPos,1.0);
}
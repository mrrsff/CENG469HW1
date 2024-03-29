#version 330 core

const int MAX_LIGHTS = 16;
layout (std140) uniform Lights
{
	int numLights;
	vec3 position[MAX_LIGHTS];
	vec3 ambient[MAX_LIGHTS];
	vec3 diffuse[MAX_LIGHTS];
	vec3 specular[MAX_LIGHTS];
	float constant[MAX_LIGHTS];
	float linear[MAX_LIGHTS];
	float quadratic[MAX_LIGHTS];
};

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 projection;
	vec3 eyePos;
};

uniform mat4 model;
uniform sampler2D tex;

layout(location=0) in vec3 inVertex; // the position of the fragment in world space
layout(location=1) in vec3 inNormal; // normal in world space
layout(location=2) in vec2 inTexCoords; // texture coordinates

out vec4 color;

void main(void)
{	
	// White color
	color = vec4(1.0, 1.0, 1.0, 1.0);

	gl_Position = projection * view * model * vec4(inVertex.xyz, 1.0);
}
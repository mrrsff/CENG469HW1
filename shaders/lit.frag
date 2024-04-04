#version 330 core

// Flat shading fragment shader
struct Light
{
	vec3 position;
	float quadratic;
	vec3 ambient;
	float linear;
	vec3 diffuse;
	float constant;
	vec3 specular;
	vec3 colorIntensity;
};

const int MAX_LIGHTS = 32;
layout (std140) uniform Lights
{
	int numLights;
	Light lights[MAX_LIGHTS];
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

uniform Material material;


in vec3 fragEyePos;
in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;


void main(void)
{
	vec3 normal = normalize(fragWorldNor);
	vec3 viewDir = normalize(fragEyePos - fragWorldPos.xyz);
	vec3 result = vec3(0.0);
	for (int i = 0; i < numLights; i++)
	{
		Light light = lights[i];
		vec3 lightDir = normalize(light.position - fragWorldPos.xyz);
		float diff = max(dot(normal, lightDir), 0.0);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 ambient = light.ambient * material.ambient;
		vec3 diffuse = light.diffuse * (diff * material.diffuse);
		vec3 specular = light.specular * (spec * material.specular);

		result += (ambient + diffuse + specular) * light.colorIntensity;
	}
	fragColor = vec4(result, 1.0);
}

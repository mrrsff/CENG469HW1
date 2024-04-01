#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "typedefs.h"
#include "printExtensions.h"

class Light {
public:
	int id;
	Vector3 position; // position of the light source
	Vector3 ambient; // ambient color of the light source
	Vector3 diffuse; // diffuse color of the light source 
	Vector3 specular; // specular color of the light source
	Vector3 colorIntensity; // color of the light source
	float constant; // constant attenuation factor
	float linear; // linear attenuation factor
	float quadratic; // quadratic attenuation factor

	Light()
	{
		id = 0;
		position = Vector3(0.0f, 0.0f, 0.0f);
		ambient = Vector3(0.0f, 0.0f, 0.0f);
		diffuse = Vector3(0.0f, 0.0f, 0.0f);
		specular = Vector3(0.0f, 0.0f, 0.0f);
		constant = 0.0f;
		linear = 0.0f;
		quadratic = 0.0f;
		colorIntensity = Vector3(0.0f, 0.0f, 0.0f);
	}

	Light(int id)
	{
		this->id = id;
		position = Vector3(0.0f, 0.0f, 0.0f);
		ambient = Vector3(0.0f, 0.0f, 0.0f);
		diffuse = Vector3(0.0f, 0.0f, 0.0f);
		specular = Vector3(0.0f, 0.0f, 0.0f);
		constant = 0.0f;
		linear = 0.0f;
		quadratic = 0.0f;
		colorIntensity = Vector3(0.0f, 0.0f, 0.0f);
	}
	Light(int id, const Vector3& position, const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, float constant, float linear, float quadratic, const Vector3& colorIntensity)
	{
		this->id = id;
		this->position = position;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
		this->colorIntensity = colorIntensity;
	}
};

#endif
#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "typedefs.h"

class Light {
public:
	int id;
	Vector3 position;
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;
	Vector3 color;
	float constant;
	float linear;
	float quadratic;

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
	
	}
	Light(int id, const Vector3& position, const Vector3& ambient, const Vector3& diffuse, const Vector3& specular, float constant, float linear, float quadratic)
	{
		this->id = id;
		this->position = position;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic;
	}
};

#endif
#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "Light.h"
#include "Camera.h"

#include <vector>
#include <string>
#include <iostream>

// Lights: binding = 0
// Camera: binding = 1
const int MAX_LIGHTS = 16;
struct __lights {
	int numLights;
	Vector3 position[MAX_LIGHTS];
	Vector3 ambient[MAX_LIGHTS];
	Vector3 diffuse[MAX_LIGHTS];
	Vector3 specular[MAX_LIGHTS];
	float constant[MAX_LIGHTS];
	float linear[MAX_LIGHTS];
	float quadratic[MAX_LIGHTS];
};

struct __camera {
	Matrix4 view;
	Matrix4 projection;
	Vector3 eyePos;
};

class MeshRenderer {
public:
	MeshRenderer();
	~MeshRenderer();

	void SetLights(const std::vector<Light>& lights);
	void SetCamera(Camera* camera);
	void Draw(GameObject& gameObject);

private:
	GLuint lightUBO;
	GLuint cameraUBO;
	__lights lightsData;
	__camera cameraData;
	std::vector<Light> lights;
	Camera* camera;
	void setupLightsUBO();
	void setupCameraUBO();
};

#endif
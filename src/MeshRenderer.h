#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "GameObject.h"
#include "Light.h"
#include "Camera.h"
#include "utils.h"
#include "printExtensions.h"

#include <vector>
#include <string>
#include <iostream>

// Lights: binding = 0
// Camera: binding = 1
const int MAX_LIGHTS = 32;
struct __light {
    Vector3 position;
	float quadratic;
    Vector3 ambient;
	float linear; 
    Vector3 diffuse;
	float constant;
    Vector3 specular;      
	float padding; // Aligns to 16 bytes
	Vector3 colorIntensity;
	float padding2; // Aligns to 16 bytes
};
struct __lights {
	int numLights;
    int _pad[3]; // Padding to align next member to 16 bytes
    __light lightSources[MAX_LIGHTS]; // Assuming MAX_LIGHTS is defined
    __lights() {
        // Initialize padding to ensure it doesn't contain garbage values
        for (int i = 0; i < 3; ++i) {
            _pad[i] = 0;
        }
    }
};

struct __camera {
	Matrix4 view;
	Matrix4 projection;
	// add 4 bytes padding
	float padding;
	Vector3 eyePos;
};

class MeshRenderer {
public:
	MeshRenderer();
	~MeshRenderer();

	void SetLights(const std::vector<Light>& lights);
	void SetCamera(Camera* camera);
	void Draw(GameObject& gameObject);

	void DebugLightUBO();
	void DebugCameraUBO();

	void UpdateCameraUBO();

private:
	GLvoid* lightsDataPtr;
	GLvoid* cameraDataPtr;
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
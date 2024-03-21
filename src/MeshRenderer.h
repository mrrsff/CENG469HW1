#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "GameObject.h"

#include <vector>

class Light;
class Camera;

class MeshRenderer {
public:
	MeshRenderer();
	~MeshRenderer();

	void AddLight(const Light& light);
	void RemoveLight(const Light& light);
	void ClearLights();
	void Draw(const GameObject& gameObject, const Camera& camera);

private:
	std::vector<Light> lights;
};
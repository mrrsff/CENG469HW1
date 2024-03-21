#include "MeshRenderer.h"

#include "Light.h"

MeshRenderer::MeshRenderer() {
	lights = std::vector<Light>();
}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::AddLight(const Light& light) {
	lights.push_back(light);
}

void MeshRenderer::RemoveLight(const Light& light) {
	lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}

void MeshRenderer::ClearLights() {
	lights.clear();
}

void MeshRenderer::Draw(const GameObject& gameObject, const Camera& camera) {
	gameObject.shader.use();
	gameObject.shader.setMat4("model", gameObject.model);
	gameObject.shader.setMat4("view", camera.view);
	gameObject.shader.setMat4("projection", camera.projection);
	gameObject.shader.setVec3("viewPos", camera.position);
	for (int i = 0; i < lights.size(); i++) {
		gameObject.shader.setVec3("lights[" + std::to_string(i) + "].position", lights[i].position);
		gameObject.shader.setVec3("lights[" + std::to_string(i) + "].ambient", lights[i].ambient);
		gameObject.shader.setVec3("lights[" + std::to_string(i) + "].diffuse", lights[i].diffuse);
		gameObject.shader.setVec3("lights[" + std::to_string(i) + "].specular", lights[i].specular);
		gameObject.shader.setFloat("lights[" + std::to_string(i) + "].constant", lights[i].constant);
		gameObject.shader.setFloat("lights[" + std::to_string(i) + "].linear", lights[i].linear);
		gameObject.shader.setFloat("lights[" + std::to_string(i) + "].quadratic", lights[i].quadratic);
	}
	gameObject.mesh.Draw(gameObject.shader);
}

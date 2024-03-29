#include "MeshRenderer.h"

MeshRenderer::MeshRenderer() {
	lights = std::vector<Light>();
}

MeshRenderer::~MeshRenderer() {}

void MeshRenderer::SetLights(const std::vector<Light>& lights) {
	this->lights = lights;
	setupLightsUBO();
}

void MeshRenderer::SetCamera(Camera* camera) {
	this->camera = camera;
	setupCameraUBO();
}

void MeshRenderer::Draw(GameObject& gameObject) {
	ShaderProgram* shader = gameObject.shader;
	shader->use();

	// Bind the lights UBO
	GLuint lightUBOIndex = glGetUniformBlockIndex(shader->getID(), "Lights");
	glUniformBlockBinding(shader->getID(), lightUBOIndex, 0);
	assert(glGetError() == GL_NO_ERROR);
	
	// Bind the camera UBO
	GLuint cameraUBOIndex = glGetUniformBlockIndex(shader->getID(), "Matrices");
	glUniformBlockBinding(shader->getID(), cameraUBOIndex, 1);
	assert(glGetError() == GL_NO_ERROR);

	shader->setMat4("model", gameObject.getModelingMatrix());

	gameObject.mesh->Draw();

	shader->unuse();
}

void MeshRenderer::setupLightsUBO() {
	/*
		const int MAX_LIGHTS = 16;
		layout (std140, binding = 0) uniform Lights
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
	*/
	lightsData.numLights = lights.size();
	for (int i = 0; i < lights.size(); i++) {
		lightsData.position[i] = lights[i].position;
		lightsData.ambient[i] = lights[i].ambient;
		lightsData.diffuse[i] = lights[i].diffuse;
		lightsData.specular[i] = lights[i].specular;
		lightsData.constant[i] = lights[i].constant;
		lightsData.linear[i] = lights[i].linear;
		lightsData.quadratic[i] = lights[i].quadratic;
	}
	
	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lightsData), &lightsData, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	assert(glGetError() == GL_NO_ERROR);
}

void MeshRenderer::setupCameraUBO() {
	/*
		layout (std140, binding = 1) uniform Matrices
		{
			mat4 view;
			mat4 projection;
			vec3 eyePos;
		};
	*/
	cameraData.view = camera->getViewMatrix();
	cameraData.projection = camera->getProjectionMatrix();
	cameraData.eyePos = camera->getPosition();

	glGenBuffers(1, &cameraUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(cameraData), &cameraData, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	assert(glGetError() == GL_NO_ERROR);
}
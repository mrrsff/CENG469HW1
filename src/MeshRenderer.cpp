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

void MeshRenderer::Draw(GameObject* gameObject) {
	ShaderProgram* shader = gameObject->shader;
	if (shader == nullptr)
	{
		std::cerr << "No shader attached to the game object" << std::endl;
		return;
	}
	shader->use();

	// Bind the lights UBO
	GLuint lightUBOIndex = glGetUniformBlockIndex(shader->getID(), "Lights");
	glUniformBlockBinding(shader->getID(), lightUBOIndex, 0);
	assert(glGetError() == GL_NO_ERROR);
	
	// Bind the camera UBO
	GLuint cameraUBOIndex = glGetUniformBlockIndex(shader->getID(), "CameraMatrices");
	glUniformBlockBinding(shader->getID(), cameraUBOIndex, 1);
	assert(glGetError() == GL_NO_ERROR);
	
	// Set the model matrix
	shader->setMat4("model", gameObject->getModelingMatrix());

	// Set if texture is enabled
	shader->setBool("useTexture", gameObject->mesh->textures.size() > 0);

	gameObject->mesh->Draw();

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
	lightsData = __lights();
	lightsData.numLights = lights.size();
	for (int i = 0; i < lights.size(); i++) {
		__light light = __light();
		light.position = lights[i].position;
		light.quadratic = lights[i].quadratic;

		light.ambient = lights[i].ambient;
		light.linear = lights[i].linear;

		light.diffuse = lights[i].diffuse;
		light.constant = lights[i].constant;
		
		light.specular = lights[i].specular;
		light.colorIntensity = lights[i].colorIntensity;
		lightsData.lightSources[i] = light;
	}

	
	glGenBuffers(1, &lightUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(__lights), &lightsData, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);

	// lightsDataPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(__lights), GL_MAP_READ_BIT);
	// glFlushMappedBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(__lights));
	
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
	glGenBuffers(1, &cameraUBO);
	UpdateCameraUBO();
}

void MeshRenderer::UpdateCameraUBO() {
	cameraData.view = camera->getViewMatrix();
	cameraData.projection = camera->getProjectionMatrix();
	cameraData.padding = 0.0f;
	cameraData.eyePos = camera->getPosition();

	int size = sizeof(__camera);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO); // Bind buffer
	glBufferData(GL_UNIFORM_BUFFER, size, &cameraData, GL_STATIC_DRAW); // Set buffer data
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, cameraUBO); // Bind buffer to binding point 1

	// cameraDataPtr = glMapBufferRange(GL_UNIFORM_BUFFER, 0, size, GL_MAP_READ_BIT);
	// glFlushMappedBufferRange(GL_UNIFORM_BUFFER, 0, size); // Flush buffer

	glBindBuffer(GL_UNIFORM_BUFFER, 0); // Unbind buffer
	assert(glGetError() == GL_NO_ERROR);
}

void MeshRenderer::DebugLightUBO()
{
	// Print the lights UBO
	std::cout << "Lights UBO" << std::endl;
	__lights* lightsData = (__lights*)lightsDataPtr;
	std::cout << "numLights: " << lightsData->numLights << std::endl;
	// for (int i = 0; i < lightsData->numLights; i++) {
	// 	std::cout << "Light " << i << std::endl;
	// 	std::cout << "position: "; printVector3(lightsData->position[i]);
	// 	std::cout << "ambient: "; printVector3(lightsData->ambient[i]);
	// 	std::cout << "diffuse: "; printVector3(lightsData->diffuse[i]);
	// 	std::cout << "specular: "; printVector3(lightsData->specular[i]);
	// 	std::cout << "constant: " << lightsData->constant[i] << std::endl;
	// 	std::cout << "linear: " << lightsData->linear[i] << std::endl;
	// 	std::cout << "quadratic: " << lightsData->quadratic[i] << std::endl;
	// }
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}
void MeshRenderer::DebugCameraUBO()
{
	// Print the camera UBO
	std::cout << "Camera UBO" << std::endl;
	__camera* cameraData = (__camera*)cameraDataPtr;
	std::cout << "view: " << std::endl;
	printMatrix4(cameraData->view);
	std::cout << "projection: " << std::endl;
	printMatrix4(cameraData->projection);
	std::cout << "eyePos: "; printVector3(cameraData->eyePos);	
}
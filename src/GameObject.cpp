#include "GameObject.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "typedefs.h"

GameObject::GameObject(const Mesh& mesh,const ShaderProgram& shader) {
	this->mesh = mesh;
	this->shader = shader;
	position = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	rotation = Quaternion(1, 0, 0, 0);
	updateModelingMatrix();
}

void GameObject::Draw() {
	shader.use();
	shader.setMat4("model", model);
	mesh.Draw(shader);
}

void GameObject::SetPosition(Vector3 position) {
	this->position = position;
	updateModelingMatrix();
}

void GameObject::SetScale(Vector3 scale) {
	this->scale = scale;
	updateModelingMatrix();
}

void GameObject::SetRotation(Quaternion rotation) {
	this->rotation = rotation;
	updateModelingMatrix();
}

void GameObject::updateModelingMatrix() {
	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = model * glm::mat4_cast(rotation);
}
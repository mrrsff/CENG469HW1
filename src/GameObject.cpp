#include "GameObject.h"

GameObject::GameObject() {
	name = "GameObject";
	mesh = nullptr;
	shader = nullptr;
	position = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	rotation = Quaternion(1, 0, 0, 0);
	updateModelingMatrix();
}

GameObject::GameObject(std::string name, Mesh* mesh, ShaderProgram* shader) {
	this->name = name;
	this->mesh = mesh;
	this->shader = shader;
	position = Vector3(0, 0, 0);
	scale = Vector3(1, 1, 1);
	rotation = Quaternion(1, 0, 0, 0);
	updateModelingMatrix();
}

Matrix4 GameObject::getModelingMatrix() {
	if (modelingMatrixDirty) {
		updateModelingMatrix();
	}
	return modelingMatrix;
}

void GameObject::SetMesh(Mesh* mesh) {
	this->mesh = mesh;
}

void GameObject::SetShader(ShaderProgram* shader) {
	this->shader = shader;
}

void GameObject::SetPosition(Vector3 position) {
	this->position = position;
	modelingMatrixDirty = true;
}

void GameObject::SetScale(Vector3 scale) {
	this->scale = scale;
	modelingMatrixDirty = true;
}

void GameObject::SetRotation(Quaternion rotation) {
	this->rotation = rotation;
	modelingMatrixDirty = true;
}

Quaternion GameObject::GetRotation() {
	return rotation;
}

Vector3 GameObject::GetPosition() {
	return position;
}

Vector3 GameObject::GetScale() {
	return scale;
}

void GameObject::updateModelingMatrix() {
	modelingMatrixDirty = false;
	modelingMatrix = glm::mat4(1.0f);
	modelingMatrix = glm::translate(modelingMatrix, position);
	modelingMatrix = glm::scale(modelingMatrix, scale);
	modelingMatrix = modelingMatrix * glm::mat4_cast(rotation);
}
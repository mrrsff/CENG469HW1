// Object.h
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "printExtensions.h"
#include <iostream>
#include <string>

class MeshRenderer;

class GameObject {
public:
	std::string name;
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;
	ShaderProgram* shader;
    Mesh* mesh;

	GameObject();
    GameObject(std::string name, Mesh* mesh, ShaderProgram* shader);

	Matrix4 getModelingMatrix();
	
	void SetMesh(Mesh* mesh);
	void SetShader(ShaderProgram* shader);
	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale);
	void SetRotation(Quaternion rotation);

	Quaternion GetRotation();
	Vector3 GetPosition();
	Vector3 GetScale();

	friend std::ostream& operator<<(std::ostream& os, const GameObject& gameObject) {
		os << "GameObject: " << gameObject.name << std::endl;
		os << "Position: " << gameObject.position.x << ", " << gameObject.position.y << ", " << gameObject.position.z << std::endl;
		os << "Scale: " << gameObject.scale.x << ", " << gameObject.scale.y << ", " << gameObject.scale.z << std::endl;
		os << "Rotation: " << gameObject.rotation.x << ", " << gameObject.rotation.y << ", " << gameObject.rotation.z << ", " << gameObject.rotation.w << std::endl;
		os << *gameObject.mesh << *gameObject.shader;
		return os;
	}

private:
	bool modelingMatrixDirty;
	Matrix4 modelingMatrix;
	void updateModelingMatrix();
};

#endif

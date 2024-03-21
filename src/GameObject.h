// Object.h
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"

class MeshRenderer;

class GameObject {
	friend class MeshRenderer;
public:
    Mesh mesh;
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;
	ShaderProgram shader;

    GameObject(const Mesh& mesh,const ShaderProgram& shader);

	void Draw();

	// TODO: trigger this with dirty flag
	// void OnUpdate();
	
	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale);
	void SetRotation(Quaternion rotation);

private:
	bool m_dirty = true; // Make this useful
	Matrix4 modelingMatrix;
	void updateModelingMatrix();
};

#endif

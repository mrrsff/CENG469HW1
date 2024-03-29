// File: Mesh.cpp
#include "Mesh.h"

Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Face> faces) {
	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->faces = faces;
	this->setupMesh();
}
void Mesh::Draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), &vertices[0], GL_STATIC_DRAW);

	// Faces
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(Face), &faces[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)offsetof(Vector3, x));

	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)offsetof(Texture, u));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	assert(glGetError() == GL_NO_ERROR);
}
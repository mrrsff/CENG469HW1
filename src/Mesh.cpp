// File: Mesh.cpp
#include "Mesh.h"

Mesh::Mesh() {
	this->vertices = std::vector<Vector3>();
	this->normals = std::vector<Vector3>();
	this->textures = std::vector<Texture>();
	this->faces = std::vector<Triangle>();
	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3> vertices) {
	this->vertices = vertices;
	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Triangle> faces) {
	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->faces = faces;
	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Texture> textures, std::vector<Triangle> faces) {
	this->vertices = vertices;
	this->textures = textures;
	this->faces = faces;
	calculateNormals();

	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Triangle> faces) {
	this->vertices = vertices;
	this->faces = faces;
	calculateNormals();

	this->setupMesh();
}
Mesh::Mesh(Mesh* mesh) {
	for(Vector3 v : mesh->vertices) {
		this->vertices.push_back(v);
	}
	for(Vector3 n : mesh->normals) {
		this->normals.push_back(n);
	}
	for(Texture t : mesh->textures) {
		this->textures.push_back(t);
	}
	for(Triangle f : mesh->faces) {
		this->faces.push_back(f);
	}
	this->setupMesh();
}
void Mesh::UpdateMesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Triangle> faces) {
	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->faces = faces;
	this->setupMesh();
}
void Mesh::UpdateMesh() {
	this->setupMesh();
}
void Mesh::Draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
	// Each face has 3 vertices, so we multiply by 3.
	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate memory for vertices, normals, textures
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size() + textures.size()) * sizeof(Vector3), 0, GL_STATIC_DRAW);
	// copy vertices, normals, textures to VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vector3), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), normals.size() * sizeof(Vector3), &normals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(Vector3), textures.size() * sizeof(Texture), &textures[0]);

	// Indices indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	std::vector<int> indices;
	for (int i = 0; i < faces.size(); ++i) {
		indices.push_back(faces[i].vIndex[0]);
		indices.push_back(faces[i].vIndex[1]);
		indices.push_back(faces[i].vIndex[2]);
	}
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	
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

void Mesh::DebugMeshInfo() {
	std::cout << "Mesh(DEBUG): " << std::endl;
	// Print all vertices, all normals, all textures, all faces
	std::cout << "Vertices count: " << vertices.size() << std::endl;
	for (int i = 0; i < vertices.size(); ++i) {
		std::cout << "Vertex " << i << ": "; printVector3(vertices[i]);
	}
	std::cout << "Normals count: " << normals.size() << std::endl;
	for (int i = 0; i < normals.size(); ++i) {
		std::cout << "Normal " << i << ": "; printVector3(normals[i]);
	}
	std::cout << "Textures count: " << textures.size() << std::endl;
	for (int i = 0; i < textures.size(); ++i) {
		std::cout << "Texture " << i << ": " << textures[i].u << ", " << textures[i].v << std::endl;
	}
	std::cout << "Faces count: " << faces.size() << std::endl;
	for (int i = 0; i < faces.size(); ++i) {
		std::cout << "Face " << i << ": " << faces[i].vIndex[0] << ", " << faces[i].vIndex[1] << ", " << faces[i].vIndex[2] << std::endl;
	}

	// Print EBO contents
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// int* data = (int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
	// std::cout << "EBO contents: " << std::endl;
	// for (int i = 0; i < faces.size() * 3; ++i) {
	// 	if (i % 3 == 0) std::cout << std::endl;
	// 	std::cout << data[i] << " ";
	// }
	// std::cout << std::endl;

}

void Mesh::calculateNormals() {
	// Calculate normals
	for (int i = 0; i < faces.size(); ++i) {
		Vector3 v0 = vertices[faces[i].vIndex[0]];
		Vector3 v1 = vertices[faces[i].vIndex[1]];
		Vector3 v2 = vertices[faces[i].vIndex[2]];

		Vector3 normal = cross(v1 - v0, v2 - v0);
		normal = normalize(normal);

		normals.push_back(normal);
	}
}
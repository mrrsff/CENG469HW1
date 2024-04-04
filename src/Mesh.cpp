// File: Mesh.cpp
#include "Mesh.h"
Mesh::Mesh() {
}
Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Quad> quads)
{
	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->quads = quads;
	quadMesh = true;

	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Triangle> triangles) {
	this->vertices = vertices;
	this->normals = normals;
	this->textures = textures;
	this->triangles = triangles;
	quadMesh = false;

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
	for(Triangle f : mesh->triangles) {
		this->triangles.push_back(f);
	}
	for(Quad q : mesh->quads) {
		this->quads.push_back(q);
	}
	this->quadMesh = mesh->quadMesh;
	this->setupMesh();
}
void Mesh::UpdateMesh() {
	this->setupMesh();
}
void Mesh::Draw() {
	if (m_dirty) {
		setupMesh();
	}
	glBindVertexArray(VAO);
	assert(glGetError() == GL_NO_ERROR);

	glDrawElements(GL_TRIANGLES, glIndices.size(), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
}
void Mesh::setupMesh() {
	m_dirty = false;

	// To use flat shading, we create 3 vertex per face.
	// We need to calculate the normals of each face and assign it to each vertex of that face.
	// But since a vertex is shared by multiple faces, we need to create new vertices for each face.
	// This is why we have to create new vertices for each face.
	// We also need to create new normals for each face.

	// Before creating new vertices, we need check if the mesh is a quad mesh, if it is, we need to convert it to a triangle mesh.
	if (this->quadMesh)
	{
		std::vector<Triangle> newTriangles;
		newTriangles.reserve(quads.size() * 2);
		for (Quad q : quads)
		{
			Triangle t1 = Triangle({q.vIndex[0], q.vIndex[1], q.vIndex[2]});
			Triangle t2 = Triangle({q.vIndex[0], q.vIndex[2], q.vIndex[3]});

			newTriangles.push_back(t1);
			newTriangles.push_back(t2);
		}
		this->triangles = newTriangles;
	}

	// Create new vertices and normals
	std::vector<Vector3> newVertices;
	std::vector<Vector3> newNormals;
	std::vector<Texture> newTextures;
	std::vector<int> newIndices;

	newVertices.reserve(triangles.size() * 3);
	newNormals.reserve(triangles.size() * 3);
	newTextures.reserve(triangles.size() * 3);
	newIndices.reserve(triangles.size() * 3);
	int index = 0;
	for (Triangle t : triangles)
	{
		Vector3 v1 = vertices[t.vIndex[0]];
		Vector3 v2 = vertices[t.vIndex[1]];
		Vector3 v3 = vertices[t.vIndex[2]];
		Vector3 v21 = v2 - v1;
		Vector3 v31 = v3 - v1;
		Vector3 normal = cross(v21, v31);
		normal = normalize(normal);
		for (int i = 0; i < 3; ++i)
		{
			newVertices.push_back(vertices[t.vIndex[i]]);
			newNormals.push_back(normal);
			newIndices.push_back(index++);
		}
	}

	// Create VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);	
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Cache sizes
	int verticesSize = newVertices.size() * sizeof(Vector3);
	int normalsSize = newNormals.size() * sizeof(Vector3);
	int texturesSize = 0;
	int indicesSize = newIndices.size() * sizeof(int);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize + normalsSize + texturesSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, newVertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, newNormals.data());
	glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, texturesSize, newTextures.data());

	// Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, newIndices.data(), GL_DYNAMIC_DRAW);

	// Set vertex attributes
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(verticesSize));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(verticesSize + normalsSize));
	glEnableVertexAttribArray(2);

	// Unbind VAO
	glBindVertexArray(0);

	// Cache indices
	std::cout << "newIndices size: " << newIndices.size() << std::endl;
	// Print glIndices and newIndices
	std::cout << "newIndices: " << std::endl;
	for (int i = 0; i < newIndices.size(); ++i)
	{
		std::cout << newIndices[i] << " ";
	}
	std::cout << std::endl;
	glIndices = std::vector<int>(newIndices.size());
	for (int i = 0; i < newIndices.size(); ++i)
	{
		glIndices[i] = newIndices[i];
	}
	std::cout << "glIndices: " << std::endl;
	for (int i = 0; i < glIndices.size(); ++i)
	{
		std::cout << glIndices[i] << " ";
	}
	std::cout << std::endl;

	// Unbind VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(glGetError() == GL_NO_ERROR);
	std::cout << "Mesh: " << newVertices.size() << " vertices, " << newNormals.size() << " normals, " << textures.size() << " textures, " << triangles.size() << " triangles" << std::endl;
}

void Mesh::DebugMeshInfo() {
	std::cout << "Mesh(DEBUG): " << std::endl;
	// Print all vertices, all normals, all textures, all triangles
	std::cout << "Vertices count: " << vertices.size() << std::endl;
	for (int i = 0; i < vertices.size(); ++i) {
		std::cout << "Vertex " << i << ": "; printVector3(vertices[i]);
	}
	std::cout << "Normals count: " << normals.size() << std::endl;
	for (int i = 0; i < normals.size(); ++i) {
		std::cout << "Normal " << i << ": "; printVector3(normals[i]);
	}
	// std::cout << "Textures count: " << textures.size() << std::endl;
	// for (int i = 0; i < textures.size(); ++i) {
	// 	std::cout << "Texture " << i << ": " << textures[i].u << ", " << textures[i].v << std::endl;
	// }
	// std::cout << "Triangle count: " << triangles.size() << std::endl;
	// for (int i = 0; i < triangles.size(); ++i) {
	// 	std::cout << "Triangle " << i << ": " << triangles[i].vIndex[0] << ", " << triangles[i].vIndex[1] << ", " << triangles[i].vIndex[2] << std::endl;
	// }

	// Print EBO contents
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// int* data = (int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
	// std::cout << "EBO contents: " << std::endl;
	// for (int i = 0; i < faces.size() * 3; ++i) {
	// 	if (i % 3 == 0) std::cout << std::endl;
	// 	std::cout << data[i] << " ";
	// }
	// std::cout << std::endl;

	// Print VBO contents of normals
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Vector3* data = (Vector3*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	// std::cout << "VBO contents: " << std::endl;
	// std::cout << "Vertices count: " << vertices.size() << std::endl;
	// for (int i = 0; i < vertices.size(); ++i) {
	// 	std::cout << "Vertex " << i << ": "; printVector3(data[i]);
	// }
	// std::cout << "Normals count: " << normals.size() << std::endl;
	// for (int i = 0; i < normals.size(); ++i) {
	// 	std::cout << "Normal " << i << ": "; printVector3(data[vertices.size() + i]);
	// }
	// for (int i = 0; i < textures.size(); ++i) {
	// 	std::cout << "Texture " << i << ": " << data[vertices.size() + normals.size() + i].x << ", " << data[vertices.size() + normals.size() + i].y << std::endl;
	// }
	// glUnmapBuffer(GL_ARRAY_BUFFER);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
}

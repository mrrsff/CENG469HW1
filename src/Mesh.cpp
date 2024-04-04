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

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
}
void Mesh::setupMesh() {	
	// Recalculate normals to get flat shading
	std::vector<Triangle> newTriangles;
	if (quadMesh)
	{
		for (int i = 0; i < quads.size(); ++i)
		{
			newTriangles.push_back(Triangle({quads[i].vIndex[0], quads[i].vIndex[1], quads[i].vIndex[2]}));
			newTriangles.push_back(Triangle({quads[i].vIndex[0], quads[i].vIndex[2], quads[i].vIndex[3]}));
		}
	}
	else
	{
		newTriangles = triangles;
	}

	std::vector<int> newIndices(newTriangles.size() * 3);
	std::vector<Vector3> newNormals(newTriangles.size() * 3);
	std::vector<Vector3> newVertices(newTriangles.size() * 3);
	for (int i = 0; i < newTriangles.size(); ++i) {
		Vector3 v0 = vertices[newTriangles[i].vIndex[0]];
		Vector3 v1 = vertices[newTriangles[i].vIndex[1]];
		Vector3 v2 = vertices[newTriangles[i].vIndex[2]];
		Vector3 normal = cross(v1 - v0, v2 - v0);
		normal = normalize(normal);

		int index1 = i * 3;
		int index2 = i * 3 + 1;
		int index3 = i * 3 + 2;

		newVertices[index1] = v0;
		newVertices[index2] = v1;
		newVertices[index3] = v2;

		newNormals[index1] = normal;
		newNormals[index2] = normal;
		newNormals[index3] = normal;

		newIndices[index1] = index1;
		newIndices[index2] = index2;
		newIndices[index3] = index3;
	}

	indexCount = newIndices.size();

	// Delete old buffers if they exist
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	GLuint vertexSize = newVertices.size() * sizeof(Vector3);
	GLuint normalSize = newNormals.size() * sizeof(Vector3);
	GLuint textureSize = textures.size() * sizeof(Texture);
	GLuint totalSize = vertexSize + normalSize + textureSize;
	
	GLuint normalOffset = vertexSize;
	GLuint textureOffset = normalOffset + normalSize;
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate memory for vertices, normals, textures
	glBufferData(GL_ARRAY_BUFFER, totalSize, 0, GL_STATIC_DRAW);
	// copy vertices, normals, textures to VBO
	glBufferSubData(GL_ARRAY_BUFFER, 0,				vertexSize,	 &newVertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, normalOffset,	normalSize,	 &newNormals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, textureOffset,	textureSize, &textures[0]);

	// Set vertex attributes
	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)BUFFER_OFFSET(normalOffset));
	glEnableVertexAttribArray(1);
	// Textures
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Texture), (void*)BUFFER_OFFSET(textureOffset));
	glEnableVertexAttribArray(2);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newIndices.size() * sizeof(int), &newIndices[0], GL_STATIC_DRAW);

	// Unbind
	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
	m_dirty = false;
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
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	Vector3* data = (Vector3*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	std::cout << "VBO contents: " << std::endl;
	std::cout << "Vertices count: " << vertices.size() << std::endl;
	for (int i = 0; i < vertices.size(); ++i) {
		std::cout << "Vertex " << i << ": "; printVector3(data[i]);
	}
	std::cout << "Normals count: " << normals.size() << std::endl;
	for (int i = 0; i < normals.size(); ++i) {
		std::cout << "Normal " << i << ": "; printVector3(data[vertices.size() + i]);
	}
	for (int i = 0; i < textures.size(); ++i) {
		std::cout << "Texture " << i << ": " << data[vertices.size() + normals.size() + i].x << ", " << data[vertices.size() + normals.size() + i].y << std::endl;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

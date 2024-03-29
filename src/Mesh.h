// Mesh.h
#ifndef MESH_H
#define MESH_H

#include "typedefs.h"
#include "ShaderProgram.h"
#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture
{
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};
struct Face
{
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};

class Mesh {
public:
    std::vector<Vector3> vertices;
	std::vector<Texture> textures;
	std::vector<Vector3> normals;
	std::vector<Face> faces;

    Mesh(std::vector<Vector3> vertices, std::vector<Vector3> normals, std::vector<Texture> textures, std::vector<Face> faces);

    void Draw();

	friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
		os << "Mesh: " << std::endl;
		os << "Vertices count: " << mesh.vertices.size() << std::endl;
		os << "Faces count: " << mesh.faces.size() << std::endl;
		return os;
	}

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif

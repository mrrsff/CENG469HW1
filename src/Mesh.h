// Mesh.h
#ifndef MESH_H
#define MESH_H

#include "typedefs.h"
#include <vector>
#include <string>
#include <GL/glew.h>

struct Vertex {
    Vector3 Position;
    Vector3 Normal;
    Vector3 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void SetVertices(std::vector<Vertex> vertices) { this->vertices = vertices; }
    void SetIndices(std::vector<unsigned int> indices) { this->indices = indices; }
    void SetTextures(std::vector<Texture> textures) { this->textures = textures; }

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

#endif

#ifndef UTILS_H
#define UTILS_H

#include "typedefs.h"
#include "GameObject.h"
#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>

Mesh* ParseObjFile(const char* path)
{
	std::vector<Vector3> vertices = std::vector<Vector3>();
	std::vector<Texture> textures = std::vector<Texture>();
	std::vector<Vector3> normals = std::vector<Vector3>();
	std::vector<Face> faces = std::vector<Face>();
	std::fstream myfile;

	// Open the input 
	myfile.open(path, std::ios::in);

	if (myfile.is_open())
	{
		std::string curLine;

		while (getline(myfile, curLine))
		{
			std::stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			std::string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't') // texture
					{
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						textures.push_back(Texture(c1, c2));
					}
					else if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						normals.push_back(Vector3(c1, c2, c3));
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						vertices.push_back(Vector3(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c)
					{
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

					faces.push_back(Face(vIndex, tIndex, nIndex));
				}
			}

			//data += curLine;
			if (!myfile.eof())
			{
				//data += "\n";
			}
		}

		myfile.close();
	}
	else 
	{
		std::cout << "Unable to open file";
		assert(false);
	}

	if (vertices.size() != normals.size() || vertices.size() == 0 || faces.size() == 0)
	{
		std::cout << "Invalid obj file";
		assert(false);
	}

	Mesh* mesh = new Mesh(vertices, normals, textures, faces);
	return mesh;
}

ShaderProgram* CreateShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	ShaderProgram* shader = new ShaderProgram(vertexShaderPath, fragmentShaderPath);
	return shader;
}

GameObject CreateGameObject(std::string name, std::string objPath, std::string vertexShaderPath, std::string fragmentShaderPath)
{
	Mesh* mesh = ParseObjFile(objPath.c_str());
	ShaderProgram* shader = CreateShaderProgram(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
	GameObject gameObject = GameObject(name,mesh, shader);
	return gameObject;
}

void CheckGLError(const char* file, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error at " << file << ":" << line << " " << err << std::endl;
		assert(false);
	}
}

Quaternion utilsLookAt(Vector3 position, Vector3 target, Vector3 up)
{
	Matrix4 lookMatrix = glm::lookAt(position, target, up);
	Quaternion rotation = glm::quat_cast(lookMatrix);
	return rotation;
}

void printVector3Formatted(Vector3 vector)
{
	// Print the vector rounded to 2 decimal places and formatted
	std::cout << "(" << std::fixed << std::setprecision(2) << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
}

void printVector4Formatted(Vector4 vector)
{
	// Print the vector rounded to 2 decimal places and formatted
	std::cout << "(" << std::fixed << std::setprecision(2) << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
}

void printMatrix4Formatted(Matrix4 matrix)
{
	// Print the matrix rounded to 2 decimal places and formatted
	std::cout << std::fixed << std::setprecision(2) << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << std::endl;
}

void printMatrix3Formatted(Matrix3 matrix)
{
	// Print the matrix rounded to 2 decimal places and formatted
	std::cout << std::fixed << std::setprecision(2) << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << std::endl;
}

void printQuaternionFormatted(Quaternion quaternion)
{
	// Print the quaternion rounded to 2 decimal places and formatted
	std::cout << "(" << std::fixed << std::setprecision(2) << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ", " << quaternion.w << ")" << std::endl;
}

#endif
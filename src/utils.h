#ifndef UTILS_H
#define UTILS_H

#include "typedefs.h"
#include "GameObject.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cassert>
#include <algorithm>

Mesh* ParseObjFile(const char* path, bool withNormals = false);
ShaderProgram* CreateShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
GameObject CreateGameObject(std::string name, std::string objPath, std::string vertexShaderPath, std::string fragmentShaderPath);
GameObject CreateGameObject(std::string name, Mesh* mesh, std::string vertexShaderPath, std::string fragmentShaderPath);
void CheckGLError(const char* file, int line);
Quaternion utilsLookAt(Vector3 position, Vector3 target, Vector3 up);
Quaternion utilsFromAxisAngle(Vector3 axis, double angle);

#endif
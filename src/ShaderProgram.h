// ShaderProgram.h
#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "typedefs.h"

class ShaderProgram {
public:
    // Constructor reads and builds the shader
    ShaderProgram();
    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    // Use/activate the shader
    void use();
    void unuse();
    // Utility uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const Vector2 &value) const;
    void setVec3(const std::string &name, const Vector3 &value) const;
    void setVec4(const std::string &name, const Vector4 &value) const;
    void setMat3(const std::string &name, const Matrix3 &value) const;
    void setMat4(const std::string &name, const Matrix4 &value) const;
    unsigned int getID() const { return ID; }

    friend std::ostream& operator<<(std::ostream& os, const ShaderProgram& shaderProgram) {
        os << "ShaderProgram: " << shaderProgram.ID << std::endl;
        return os;
    }

private:
    unsigned int ID;
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif

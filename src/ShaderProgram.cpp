// ShaderProgram.cpp
#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() {
    ID = 0;
}

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();	
    } catch(std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        assert(false);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void ShaderProgram::use() { 
    glUseProgram(ID); 
}
void ShaderProgram::unuse() { 
    glUseProgram(0);
}
void ShaderProgram::setBool(const std::string &name, bool value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }       
    glUniform1i(loc, (int)value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setInt(const std::string &name, int value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniform1i(loc, value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setFloat(const std::string &name, float value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniform1f(loc, value); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec2(const std::string &name, const Vector2 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniform2fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec3(const std::string &name, const Vector3 &value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniform3fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setVec4(const std::string &name, const Vector4 &value) const {
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniform3fv(loc, 1, glm::value_ptr(value)); 
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setMat3(const std::string &name, const Matrix3 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false); 
    }
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    assert(glGetError() == GL_NO_ERROR);
}
void ShaderProgram::setMat4(const std::string &name, const Matrix4 &value) const { 
    int loc = glGetUniformLocation(ID, name.c_str());
    if (loc == -1) {
        std::cout << "ERROR::SHADER::UNIFORM_NOT_FOUND: " << name << std::endl;
        assert(false);
    }
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    assert(glGetError() == GL_NO_ERROR);
}

void ShaderProgram::checkCompileErrors(GLuint shader, std::string type) {
    int success;
    char infoLog[1024];
    if(type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    assert(glGetError() == GL_NO_ERROR);
}
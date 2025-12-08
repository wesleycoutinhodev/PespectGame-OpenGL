#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// ====================================================
// 1. CONSTRUTOR E COMPILAÇÃO
// ====================================================

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // --- 1. Leitura dos Arquivos de Código Fonte ---
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Garante que os streams lancem exceções em caso de falha
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Abertura
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        // Leitura do buffer para streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Fechamento dos handlers
        vShaderFile.close();
        fShaderFile.close();

        // Conversão para string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "[ERRO CRITICO] Falha ao ler arquivo de shader: " << e.what() << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // --- 2. Compilação dos Shaders ---
    unsigned int vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program (Linkagem)
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Limpeza (Shaders individuais não são mais necessários após linkagem)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// ====================================================
// 2. ATIVAÇÃO (USE)
// ====================================================

void Shader::use() {
    glUseProgram(ID);
}

// ====================================================
// 3. SETTERS DE UNIFORMS
// ====================================================

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ====================================================
// 4. UTILITÁRIOS DE DEBUG
// ====================================================

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        // Checagem de erros de compilação (Vertex/Fragment)
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "[SHADER ERROR] Compilacao (" << type << "):\n" << infoLog << std::endl;
        }
    }
    else {
        // Checagem de erros de linkagem (Program)
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "[SHADER ERROR] Linkagem (" << type << "):\n" << infoLog << std::endl;
        }
    }
}
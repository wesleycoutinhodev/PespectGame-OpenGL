#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// ====================================================
// CLASSE SHADER
// Gerencia a compilação, linkagem e ativação de programas GLSL
// ====================================================

class Shader {
public:
    // --- 1. ESTADO ---
    unsigned int ID; // ID do programa compilado no OpenGL

    // ====================================================
    // MÉTODOS PÚBLICOS
    // ====================================================

    // Construtores
    Shader() { ID = 0; }
    Shader(const char* vertexPath, const char* fragmentPath);

    // Ativa o shader para uso na renderização atual
    void use();

    // --- Utilitários de Uniforms (Envio de dados CPU -> GPU) ---
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // --- MÉTODOS INTERNOS ---
    // Verifica erros de compilação e linkagem
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
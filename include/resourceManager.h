#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "shader.h"

// Uma classe estática para gerenciar recursos (Shaders e Texturas)
class ResourceManager
{
public:
    // Armazenamento de recursos
    static std::map<std::string, Shader> Shaders;

    // Carrega (e gera) um shader programa de arquivo carregando vertex, fragment (e geometry) source code
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

    // Recupera um shader armazenado
    static Shader GetShader(std::string name);

    // Limpa todos os recursos
    static void Clear();

private:
    // Construtor privado, pois é uma classe estática pura
    ResourceManager() {}

    // Carrega e gera um shader de arquivo
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
};

#endif
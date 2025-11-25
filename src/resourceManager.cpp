#include "resourceManager.h"
#include <iostream>
#include <sstream>
#include <fstream>

// Instancia as variáveis estáticas
std::map<std::string, Shader> ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

void ResourceManager::Clear()
{
    // Deleta todos os shaders (opcional, pois o OS limpa na saída, mas boa prática)
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // Simplesmente instanciamos a classe Shader que você já tem
    // Nota: Se sua classe Shader já lê o arquivo no construtor (como a que te passei), 
    // isso fica muito simples:

    Shader shader(vShaderFile, fShaderFile);
    return shader;
}
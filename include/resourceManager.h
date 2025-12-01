#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include <string>  
#include <vector> 


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

    // Armazenamento de Texturas
    static std::map<std::string, Texture2D> Textures;

    // Carrega textura de arquivo
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

    // Pega textura pelo nome
    static Texture2D GetTexture(std::string name);

    static unsigned int LoadCubemap(std::vector<std::string> faces);

private:
    // Construtor privado, pois é uma classe estática pura
    ResourceManager() {}

    // Carrega e gera um shader de arquivo
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif
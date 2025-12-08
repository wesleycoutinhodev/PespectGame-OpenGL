#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <glad/glad.h>

#include "texture.h"
#include "shader.h"

// Forward Declaration para evitar dependência cíclica
class Mesh;

// ====================================================
// CLASSE RESOURCE MANAGER
// Classe estática (Singleton) para gerenciamento de assets
// ====================================================

class ResourceManager
{
public:
    // --- 1. CACHE DE RECURSOS (MAPS) ---
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    static std::map<std::string, Mesh*>     Models;

    // ====================================================
    // MÉTODOS PÚBLICOS
    // ====================================================

    // --- Gerenciamento de Shaders ---
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    static Shader GetShader(std::string name);

    // --- Gerenciamento de Texturas ---
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
    static Texture2D GetTexture(std::string name);

    // Carrega Cubemap (Skybox) a partir de 6 texturas
    static unsigned int LoadCubemap(std::vector<std::string> faces);

    // --- Gerenciamento de Modelos 3D ---
    static Mesh* LoadModel(const char* objPath, const char* texturePath, std::string name);
    static Mesh* GetModel(std::string name);

    // --- Utilitários ---
    // Libera toda a memória carregada (Shaders, Texturas, Modelos)
    static void Clear();

private:
    // Construtor privado (Classe totalmente estática)
    ResourceManager() {}

    // Carregadores internos (Helpers)
    static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif
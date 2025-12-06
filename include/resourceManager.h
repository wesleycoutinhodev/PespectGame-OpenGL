#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <vector>
#include <glad/glad.h>
#include "texture.h"
#include "shader.h"

class Mesh;

class ResourceManager
{
public:
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;

    static std::map<std::string, Mesh*> Models;

    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
    static Shader GetShader(std::string name);

    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
    static Texture2D GetTexture(std::string name);

    static void Clear();

    static unsigned int LoadCubemap(std::vector<std::string> faces);

    static Mesh* LoadModel(const char* objPath, const char* texturePath, std::string name);
    static Mesh* GetModel(std::string name);

private:
    ResourceManager() {}
    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif
#define STB_IMAGE_IMPLEMENTATION
#include "resourceManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stb_image/stb_image.h>
#include <vector>
#include <string>

// Instancia as variáveis estáticas
std::map<std::string, Shader> ResourceManager::Shaders;

// Instancia o map estático
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

// Implementação do carregamento (A parte pesada)
Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    // Cria objeto textura
    Texture2D texture;

    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    // Carrega imagem usando stb_image
    int width, height, nrChannels;
    // O OpenGL lê o eixo Y de baixo para cima, mas imagens são de cima para baixo.
    // stbi_set_flip_vertically_on_load(true) corrige isso se a textura ficar de ponta cabeça.
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    if (data) {
        texture.Generate(width, height, data);
    }
    else {
        std::cout << "FALHA AO CARREGAR TEXTURA: " << file << std::endl;
    }

    // Libera memória da imagem (já está na GPU)
    stbi_image_free(data);

    return texture;
}

unsigned int ResourceManager::LoadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // Detecta se é RGB ou RGBA
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

            // Carrega cada face (GL_TEXTURE_CUBE_MAP_POSITIVE_X + i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "ERRO SKYBOX: Falha ao carregar " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Configurações de filtro (para não ver linhas nas bordas)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void ResourceManager::Clear()
{
    for (auto iter : Shaders) glDeleteProgram(iter.second.ID);
    for (auto iter : Textures) glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // Simplesmente instanciamos a classe Shader que você já tem
    // Nota: Se sua classe Shader já lê o arquivo no construtor (como a que te passei), 
    // isso fica muito simples:

    Shader shader(vShaderFile, fShaderFile);
    return shader;
}
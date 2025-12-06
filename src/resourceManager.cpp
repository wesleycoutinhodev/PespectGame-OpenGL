#include "resourceManager.h" 
#include "mesh.h"            

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h> /




// Instancia as variáveis estáticas
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Mesh*> ResourceManager::Models;


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

Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    Texture2D texture;

    std::cout << "    [STB] Tentando carregar: " << file << std::endl;

    int width, height, nrChannels;


    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    if (data) {
        std::cout << "    [STB] Sucesso! Dimensoes: " << width << "x" << height
            << " | Canais: " << nrChannels << std::endl;

        // DETECÇÃO AUTOMÁTICA DE FORMATO
        if (nrChannels == 4) {
            // Tem alpha
            texture.Internal_Format = GL_RGBA;
            texture.Image_Format = GL_RGBA;
            std::cout << "    [STB] Usando formato RGBA (4 canais)" << std::endl;
        }
        else if (nrChannels == 3) {
            // RGB sem alpha
            texture.Internal_Format = GL_RGB;
            texture.Image_Format = GL_RGB;
            std::cout << "    [STB] Usando formato RGB (3 canais)" << std::endl;
        }
        else if (nrChannels == 1) {
            // Grayscale
            texture.Internal_Format = GL_RED;
            texture.Image_Format = GL_RED;
            std::cout << "    [STB] Usando formato Grayscale (1 canal)" << std::endl;
        }

        texture.Generate(width, height, data);
    }
    else {
        std::cout << "    [STB] ERRO CRITICO ao carregar: " << file << std::endl;
        std::cout << "    [STB] Motivo: " << stbi_failure_reason() << std::endl;

        // Fallback
        unsigned char fallbackData[] = { 255, 255, 255, 255 };
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
        texture.Generate(1, 1, fallbackData);
    }

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

Mesh* ResourceManager::LoadModel(const char* objPath, const char* texturePath, std::string name) {
    std::cout << "  -> Carregando modelo OBJ: " << objPath << std::endl;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath);

    if (!warn.empty()) {
        std::cout << "  -> AVISO OBJ: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "  -> ERRO OBJ: " << err << std::endl;
    }

    if (!ret) {
        std::cout << "  -> FALHA ao carregar OBJ!" << std::endl;
        return nullptr;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Processa todas as shapes (meshes)
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop pelos vértices da face
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                Vertex vertex;

                // Posição
                vertex.Position = glm::vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );

                // Normal (se existir)
                if (idx.normal_index >= 0) {
                    vertex.Normal = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                }
                else {
                    vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }

                // TexCoords (se existir)
                if (idx.texcoord_index >= 0) {
                    vertex.TexCoords = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                }
                else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
            index_offset += fv;
        }
    }

    std::cout << "  -> Modelo carregado: " << vertices.size() << " vertices, "
        << indices.size() << " indices" << std::endl;

    // Carrega a textura
    Texture2D texture = LoadTexture(texturePath, true, name + "_texture");

    // Cria a mesh
    Mesh* mesh = new Mesh(vertices, indices, texture);
    Models[name] = mesh;

    return mesh;
}

Mesh* ResourceManager::GetModel(std::string name) {
    return Models[name];
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
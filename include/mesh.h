#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "texture.h"

// ====================================================
// ESTRUTURA DE VÉRTICE
// Define o layout de memória para envio à GPU
// ====================================================

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// ====================================================
// CLASSE MESH
// Representa uma malha 3D composta por vértices e texturas
// ====================================================

class Mesh {
public:
    // --- 1. DADOS DA MALHA ---
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    Texture2D                 texture;

    // --- 2. BUFFERS DE RENDERIZAÇÃO (GPU) ---
    unsigned int VAO; // Vertex Array Object
    unsigned int VBO; // Vertex Buffer Object
    unsigned int EBO; // Element Buffer Object

    // ====================================================
    // MÉTODOS
    // ====================================================

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture2D texture);

    // Renderiza a malha usando o shader ativo
    void Draw(unsigned int shaderID);

private:
    // Configura os buffers e atributos do VAO
    void setupMesh();
};

#endif
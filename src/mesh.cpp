#include "mesh.h"
#include <cstddef> // Necessário para offsetof

// ====================================================
// 1. CONSTRUTOR
// ====================================================

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Texture2D texture)
    : vertices(vertices), indices(indices), texture(texture)
{
    this->setupMesh();
}

// ====================================================
// 2. CONFIGURAÇÃO DOS BUFFERS (SETUP)
// ====================================================

void Mesh::setupMesh() {
    // Geração de Buffers/Arrays
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    // Carregamento de dados de Vértices
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    // Carregamento de Índices (EBO)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

    // Configuração de Atributos (Vertex Attrib Pointers)
    // Layout 0: Posição (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Layout 1: Normal (vec3)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Layout 2: Coordenadas de Textura (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Desvincula VAO para evitar alterações acidentais
    glBindVertexArray(0);
}

// ====================================================
// 3. RENDERIZAÇÃO (DRAW)
// ====================================================

void Mesh::Draw(unsigned int shaderID) {
    // Vincula Textura
    glActiveTexture(GL_TEXTURE0);
    this->texture.Bind();

    // Desenha Mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT, 0);

    // Limpeza de estado
    glBindVertexArray(0);
}
#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// ====================================================
// CLASSE TEXTURE2D
// Gerencia texturas 2D (armazenamento e configuração)
// ====================================================

class Texture2D
{
public:
    // --- 1. CONFIGURAÇÃO E ESTADO ---
    unsigned int ID;
    unsigned int Width, Height;    // Dimensões da imagem carregada
    unsigned int Internal_Format;  // Formato de armazenamento (ex: GL_RGB)
    unsigned int Image_Format;     // Formato da imagem original (ex: GL_RGB)

    // ====================================================
    // MÉTODOS PÚBLICOS
    // ====================================================

    Texture2D();

    // Aloca memória na GPU e configura parâmetros da textura
    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    // Ativa a textura para o slot atual
    void Bind() const;
};

#endif
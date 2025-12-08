#include "texture.h"

// ====================================================
// 1. CONSTRUTOR
// ====================================================

Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB)
{
    glGenTextures(1, &this->ID);
}

// ====================================================
// 2. GERAÇÃO E CONFIGURAÇÃO
// ====================================================

void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    glBindTexture(GL_TEXTURE_2D, this->ID);

    // Upload dos dados de imagem para a GPU
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);

    // Configuração de Wrapping (Repetição de textura)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Configuração de Filtragem (Nearest = Estilo Pixel Art/Retro)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Unbind para evitar modificações acidentais
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ====================================================
// 3. UTILITÁRIOS
// ====================================================

void Texture2D::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}
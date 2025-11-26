#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture2D
{
public:
    unsigned int ID;
    unsigned int Width, Height; // Tamanho da imagem
    unsigned int Internal_Format; // Formato (RGB ou RGBA)
    unsigned int Image_Format;    // Formato da imagem carregada

    Texture2D();

    // Gera a textura a partir dos dados da imagem
    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    // Liga a textura para uso
    void Bind() const;
};

#endif
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

// Inclua sua classe de textura se tiver, ou use IDs inteiros
// #include "Texture.h" 

class GameObject {
public:
    // Estado do Objeto
    glm::vec3 Position, Size, Color;
    float Rotation; // Em graus, eixo Y (geralmente)
    bool IsSolid;
    bool Destroyed;
    bool UseOutline;

    // Construtor
    GameObject();
    GameObject(glm::vec3 pos, glm::vec3 size, glm::vec3 color);

    // Renderização
    // Note: Passamos o Renderer ou Shader para ele saber como se desenhar
    // O 'renderer' aqui pode ser apenas uma referência ao Shader class por enquanto
    virtual void Draw(unsigned int shaderID, unsigned int VAO_Cubo);
};

#endif
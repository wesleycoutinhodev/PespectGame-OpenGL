#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "mesh.h"

// ====================================================
// CLASSE GAMEOBJECT
// Entidade base para todos os objetos da cena
// ====================================================

class GameObject {
public:
    // --- 1. TRANSFORMAÇÃO E RENDERIZAÇÃO ---
    glm::vec3 Position, Size, Color;
    float     Rotation;
    Texture2D Sprite;
    Mesh* ObjectMesh = nullptr; // Suporte a modelos 3D (.obj)
    bool      UseOutline;

    // --- 2. ESTADO FÍSICO ---
    bool IsSolid;
    bool Destroyed;
    bool Active;

    // --- 3. TAGS DE GAMEPLAY ---
    bool IsPushable; // Caixas empurráveis
    bool IsPlate;    // Placas de pressão
    bool IsCoin;     // Itens coletáveis
    bool IsJumpPad;  // Trampolins

    // --- 4. SISTEMA DE MOVIMENTO E LÓGICA ---
    bool      IsMoving;
    bool      IsActivated;
    int       LinkID;       // Canal de comunicação (Trigger -> Receiver)
    glm::vec3 MoveAxis;     // Eixo de movimento (ex: 1,0,0 para horizontal)
    float     MoveDistance;
    float     MoveSpeed;
    float     InitialX, InitialY;
    float     LocalTime;

    // ====================================================
    // MÉTODOS
    // ====================================================

    GameObject();
    GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f));

    // Renderiza o objeto (Virtual para permitir override por classes derivadas)
    virtual void Draw(unsigned int shaderID, unsigned int cubeVAO);
};

#endif
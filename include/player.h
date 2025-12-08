#ifndef PLAYER_H
#define PLAYER_H

#include "gameObject.h"
#include "mesh.h"
#include <raudio.h> 

// ====================================================
// CLASSE PLAYER
// Estende GameObject com lógica de física e input
// ====================================================

class Player : public GameObject {
public:
    // --- 1. CONFIGURAÇÃO (STATS) ---
    float MoveSpeed;
    float JumpForce;

    // --- 2. ESTADO FÍSICO ---
    glm::vec3 Velocity;
    bool      IsGrounded;
    int       JumpCount;

    // --- 3. RECURSOS VISUAIS ---
    Mesh* PlayerMesh;

    // ====================================================
    // MÉTODOS
    // ====================================================

    // Construtores
    Player(glm::vec3 pos);
    Player(glm::vec3 pos, Mesh* mesh);

    // Lógica de Controle
    void ProcessInput(bool* keys, float dt, bool is3DMode, Sound jumpSfx);

    // Override de Renderização
    void Draw(unsigned int shaderID, unsigned int cubeVAO) override;
};

#endif
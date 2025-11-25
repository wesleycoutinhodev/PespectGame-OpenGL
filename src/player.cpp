#include "player.h"

Player::Player(glm::vec3 pos)
    : GameObject(pos, glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 0.0f, 0.0f)) {

    // ... suas configurações de velocidade ...
    MoveSpeed = 5.0f;
    JumpForce = 7.0f;

    // ATIVA A BORDA SÓ PARA O PLAYER
    this->UseOutline = true;
}

void Player::ProcessInput(bool* keys, float dt, bool is3DMode) {
    // Reseta velocidade horizontal
    Velocity.x = 0.0f;
    Velocity.z = 0.0f;

    if (is3DMode) {
        // --- MODO 3D (Terceira Pessoa) ---
        // W/S movem para frente/trás no level (Eixo X)
        if (keys[GLFW_KEY_W]) Velocity.x = MoveSpeed;  // Frente (Avança no level)
        if (keys[GLFW_KEY_S]) Velocity.x = -MoveSpeed; // Trás (Volta no level)

        // A/D movem para os lados da pista (Eixo Z)
        if (keys[GLFW_KEY_A]) Velocity.z = -MoveSpeed; // Esquerda
        if (keys[GLFW_KEY_D]) Velocity.z = MoveSpeed;  // Direita
    }
    else {
        // --- MODO 2D (Plataforma Lateral) ---
        // A/D movem para frente/trás no level (Eixo X)
        if (keys[GLFW_KEY_A]) Velocity.x = -MoveSpeed; // Esquerda da tela
        if (keys[GLFW_KEY_D]) Velocity.x = MoveSpeed;  // Direita da tela

 
    }

    // Pulo (Espaço)
    if (keys[GLFW_KEY_SPACE] && IsGrounded) {
        Velocity.y = JumpForce;
        IsGrounded = false;
    }
}
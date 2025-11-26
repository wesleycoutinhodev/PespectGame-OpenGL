#include "player.h"
#include "resourceManager.h" // <--- NECESSÁRIO PARA PEGAR A TEXTURA

Player::Player(glm::vec3 pos)
// CORREÇÃO: Agora passamos 'ResourceManager::GetTexture("block")' como 3º argumento
    : GameObject(pos, glm::vec3(0.8f, 0.8f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.0f, 0.0f)) {

    // Configurações iniciais
    MoveSpeed = 5.0f;
    JumpForce = 7.0f;
    Velocity = glm::vec3(0.0f);
    IsGrounded = false;

    // ATIVA A BORDA SÓ PARA O PLAYER
    this->UseOutline = true;
}

void Player::ProcessInput(bool* keys, float dt, bool is3DMode) {
    // Reseta velocidade horizontal
    Velocity.x = 0.0f;
    Velocity.z = 0.0f;

    if (is3DMode) {
        // --- MODO 3D (Terceira Pessoa) ---
        if (keys[GLFW_KEY_W]) Velocity.x = MoveSpeed;  // Frente (Fundo)
        if (keys[GLFW_KEY_S]) Velocity.x = -MoveSpeed; // Trás (Tela)
        if (keys[GLFW_KEY_A]) Velocity.z = -MoveSpeed;  // Esquerda
        if (keys[GLFW_KEY_D]) Velocity.z = MoveSpeed; // Direita
    }
    else {
        // --- MODO 2D (Side Scroller) ---
        if (keys[GLFW_KEY_A]) Velocity.x = -MoveSpeed; // Esquerda
        if (keys[GLFW_KEY_D]) Velocity.x = MoveSpeed;  // Direita

        // Pulo (Só no 2D)
        if (keys[GLFW_KEY_SPACE] && IsGrounded) {
            Velocity.y = JumpForce;
            IsGrounded = false;
        }
    }
}
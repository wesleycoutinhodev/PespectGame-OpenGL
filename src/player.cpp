#include "player.h"
#include "resourceManager.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <GLFW/glfw3.h>

// ====================================================
// 1. CONSTRUTORES
// ====================================================

Player::Player(glm::vec3 pos)
    : GameObject(pos, glm::vec3(0.8f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.0f, 0.0f)),
    PlayerMesh(nullptr)
{
    // Configuração Padrão
    this->MoveSpeed = 5.0f;
    this->JumpForce = 7.0f;
    this->Velocity = glm::vec3(0.0f);
    this->IsGrounded = false;
    this->JumpCount = 0;
    this->UseOutline = true;
}

Player::Player(glm::vec3 pos, Mesh* mesh)
    : GameObject(pos, glm::vec3(0.8f), ResourceManager::GetTexture("block"), glm::vec3(0.7f)),
    PlayerMesh(mesh)
{
    // Configuração com Mesh 3D
    this->MoveSpeed = 5.0f;
    this->JumpForce = 7.0f;
    this->Velocity = glm::vec3(0.0f);
    this->IsGrounded = false;
    this->JumpCount = 0;
    this->UseOutline = true;
}

// ====================================================
// 2. LÓGICA DE INPUT E FÍSICA (UPDATE)
// ====================================================

void Player::ProcessInput(bool* keys, float dt, bool is3DMode, Sound jumpSfx) {
    // Reseta velocidade horizontal a cada frame para evitar inércia infinita (movimento estilo arcade)
    Velocity.x = 0.0f;
    Velocity.z = 0.0f;

    // --- Lógica de Movimentação ---
    if (is3DMode) {
        // Modo 3D (Top-Down / Isometrico)
        // Mapeia WASD para X e Z
        if (keys[GLFW_KEY_W]) Velocity.x = MoveSpeed;
        if (keys[GLFW_KEY_S]) Velocity.x = -MoveSpeed;
        if (keys[GLFW_KEY_A]) Velocity.z = -MoveSpeed;
        if (keys[GLFW_KEY_D]) Velocity.z = MoveSpeed;
    }
    else {
        // Modo 2D (Side Scroller)
        // Mapeia A/D apenas para X
        if (keys[GLFW_KEY_A]) Velocity.x = -MoveSpeed;
        if (keys[GLFW_KEY_D]) Velocity.x = MoveSpeed;

        // Mecânica de Pulo (Apenas no 2D)
        static bool spacePressed = false;

        if (keys[GLFW_KEY_SPACE]) {
            if (!spacePressed) {
                // Permite pulo se estiver no chão OU pulo duplo (JumpCount < 2)
                if (IsGrounded || JumpCount < 2) {
                    Velocity.y = JumpForce;
                    IsGrounded = false;
                    JumpCount++;
                    PlaySound(jumpSfx);
                }
                spacePressed = true; // Debounce: Trava até soltar a tecla
            }
        }
        else {
            spacePressed = false; // Destrava
        }
    }

    // --- Cálculo de Rotação do Modelo ---
    // Rotaciona o personagem para olhar na direção do movimento
    if (Velocity.x != 0.0f || Velocity.z != 0.0f) {
        float angle = glm::degrees(atan2(Velocity.x, Velocity.z));
        float modelCorrection = -90.0f; // Ajuste dependendo da orientação original do modelo 3D

        this->Rotation = angle + modelCorrection;
    }
}

// ====================================================
// 3. RENDERIZAÇÃO (DRAW)
// ====================================================

void Player::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    // 1. Efeito Visual de Flutuação (Idle Animation)
    float time = (float)glfwGetTime();
    float hoverOffset = sin(time * 4.0f) * 0.1f;

    // 2. Construção da Matriz de Modelo (TRS)
    glm::mat4 model = glm::mat4(1.0f);

    // Translação (Posição + Offset de Animação)
    model = glm::translate(model, glm::vec3(this->Position.x, this->Position.y + hoverOffset, this->Position.z));

    // Rotação (Calculada no Input)
    model = glm::rotate(model, glm::radians(this->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // Escala
    model = glm::scale(model, this->Size);

    // 3. Envio de Uniforms para GPU
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &this->Color[0]);

    // 4. Desenho da Geometria
    if (this->PlayerMesh != nullptr) {
        // Renderiza Mesh 3D Complexa
        this->PlayerMesh->Draw(shaderID);
    }
    else {
        // Renderiza Cubo Fallback
        glActiveTexture(GL_TEXTURE0);
        this->Sprite.Bind();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}
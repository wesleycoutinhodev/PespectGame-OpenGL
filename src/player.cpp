#include "player.h"
#include "resourceManager.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <iostream>
#include <GLFW/glfw3.h>

Player::Player(glm::vec3 pos)
    : GameObject(pos, glm::vec3(0.8f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.0f, 0.0f)),
    PlayerMesh(nullptr)
{
    MoveSpeed = 5.0f;
    JumpForce = 7.0f;
    Velocity = glm::vec3(0.0f);
    IsGrounded = false;
    JumpCount = 0;
    this->UseOutline = true;
}

// NOVO CONSTRUTOR COM MESH
Player::Player(glm::vec3 pos, Mesh* mesh)
    : GameObject(pos, glm::vec3(0.8f), ResourceManager::GetTexture("block"), glm::vec3(0.7f)),
    PlayerMesh(mesh)
{
    MoveSpeed = 5.0f;
    JumpForce = 7.0f;
    Velocity = glm::vec3(0.0f);
    IsGrounded = false;
    JumpCount = 0;
    this->UseOutline = true;
}

void Player::ProcessInput(bool* keys, float dt, bool is3DMode, Sound jumpSfx) {
    static bool spacePressed = false;
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
        if (keys[GLFW_KEY_SPACE]) {
            if (!spacePressed) {
                if (IsGrounded || JumpCount < 2) {
                    Velocity.y = JumpForce;
                    IsGrounded = false;
                    JumpCount++;
                    PlaySound(jumpSfx);
                    std::cout << "BOING!" << std::endl;

                }
                spacePressed = true; // Trava até soltar a tecla
            }
        }
        else {
            spacePressed = false; // Destrava quando solta a tecla
        }
    }

    // --- 2. LÓGICA DE ROTAÇÃO UNIFICADA ---
    if (Velocity.x != 0.0f || Velocity.z != 0.0f) {
        float angle = glm::degrees(atan2(Velocity.x, Velocity.z));
        float modelCorrection = -90.0f;

        this->Rotation = angle + modelCorrection;
    }

}

void Player::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    // 1. CALCULAR FLUTUAÇÃO (Apenas Visual)
    // Cria uma onda suave baseada no tempo
    float time = (float)glfwGetTime();
    float hoverOffset = sin(time * 4.0f) * 0.1f; // Velocidade 4.0, Amplitude 0.1

    // 2. CONFIGURAR MATRIZ MODEL
    glm::mat4 model = glm::mat4(1.0f);

    // Translação: Posição Física + Offset de Flutuação no Y
    model = glm::translate(model, glm::vec3(this->Position.x, this->Position.y + hoverOffset, this->Position.z));

    // Rotação: Usa o ângulo calculado no ProcessInput
    model = glm::rotate(model, glm::radians(this->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // Escala
    model = glm::scale(model, this->Size);

    // 3. ENVIAR PARA O SHADER
    // Como sua assinatura usa IDs crus (uint), usamos funções diretas do OpenGL
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &this->Color[0]);

    // 4. DESENHO (Mesh ou Cubo Fallback)
    if (this->PlayerMesh != nullptr) {
        // Se o modelo 3D carregou, desenha ele
        this->PlayerMesh->Draw(shaderID);
    }
    else {
        // Fallback: Desenha o cubo padrão se não tiver modelo
        glActiveTexture(GL_TEXTURE0);
        this->Sprite.Bind();
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}
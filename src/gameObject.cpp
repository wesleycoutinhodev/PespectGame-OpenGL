#include "gameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

// ====================================================
// 1. CONSTRUTORES
// ====================================================

GameObject::GameObject()
    : // Transform
    Position(0.0f), Size(1.0f), Rotation(0.0f), Color(1.0f),
    // Render
    Sprite(), ObjectMesh(nullptr), UseOutline(false),
    // Estado Físico
    IsSolid(false), Destroyed(false), Active(true),
    // Propriedades de Gameplay
    IsPushable(false), IsPlate(false), IsCoin(false), IsJumpPad(false),
    IsMoving(false), IsActivated(false),
    // Atributos de Movimento/Link
    MoveAxis(0.0f), MoveDistance(0.0f), MoveSpeed(0.0f),
    InitialX(0.0f), InitialY(0.0f), LinkID(0), LocalTime(0.0f)
{
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 color)
    : // Transform
    Position(pos), Size(size), Rotation(0.0f), Color(color),
    // Render
    Sprite(sprite), ObjectMesh(nullptr), UseOutline(false),
    // Estado Físico
    IsSolid(true), Destroyed(false), Active(true),
    // Propriedades de Gameplay
    IsPushable(false), IsPlate(false), IsCoin(false), IsJumpPad(false),
    IsMoving(false), IsActivated(false),
    // Atributos de Movimento/Link
    MoveAxis(0.0f), MoveDistance(0.0f), MoveSpeed(0.0f),
    InitialX(0.0f), InitialY(0.0f), LinkID(0), LocalTime(0.0f)
{
}

// ====================================================
// 2. RENDERIZAÇÃO (DRAW)
// ====================================================

void GameObject::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    // 1. Calcula a Matriz de Modelo (TRS: Translate -> Rotate -> Scale)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->Position);

    // Rotação especial automática para moedas
    if (this->IsCoin) {
        float time = (float)glfwGetTime();
        model = glm::rotate(model, time * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // Rotação padrão do objeto
    model = glm::rotate(model, glm::radians(this->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    // Escala
    model = glm::scale(model, this->Size);

    // 2. Envia Uniforms para o Shader
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &this->Color[0]);

    // 3. Renderização da Geometria
    if (this->ObjectMesh != nullptr) {
        // Opção A: Renderiza Mesh 3D importada (.obj)
        this->ObjectMesh->Draw(shaderID);
    }
    else {
        // Opção B: Renderiza Cubo Padrão com Textura 2D (Fallback)
        glActiveTexture(GL_TEXTURE0);
        this->Sprite.Bind();

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}
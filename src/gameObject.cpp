#include "gameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject()
    : Position(0.0f), Size(1.0f), Color(1.0f), Rotation(0.0f), Sprite(),
    IsSolid(false), Destroyed(false), UseOutline(false),
    IsPushable(false), IsPlate(false), IsCoin(false),
    Active(true), IsMoving(false), MoveAxis(0.0f), MoveDistance(0.0f), MoveSpeed(0.0f), InitialX(0.0f), InitialY(0.0f)
{
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 color)
    : Position(pos), Size(size), Color(color), Sprite(sprite), Rotation(0.0f),
    IsSolid(true), Destroyed(false), UseOutline(false),
    IsPushable(false), IsPlate(false), IsCoin(false),
    Active(true), IsMoving(false), MoveAxis(0.0f), MoveDistance(0.0f), MoveSpeed(0.0f), InitialX(0.0f), InitialY(0.0f)
{
}

void GameObject::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->Position);
    model = glm::rotate(model, glm::radians(this->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, this->Size);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &this->Color[0]);

    glActiveTexture(GL_TEXTURE0);
    this->Sprite.Bind();

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
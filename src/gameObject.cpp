#include "gameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GameObject::GameObject()
    : Position(0.0f), Size(1.0f), Color(1.0f), Rotation(0.0f), IsSolid(false), Destroyed(false), UseOutline(false) {
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, glm::vec3 color)
    : Position(pos), Size(size), Color(color), Rotation(0.0f), IsSolid(true), Destroyed(false), UseOutline(false) {
}

// 2. ATUALIZE O DRAW
void GameObject::Draw(unsigned int shaderID, unsigned int VAO_Cubo) {
    // ... (Cálculo da Matriz Model igual antes) ...
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->Position);
    model = glm::rotate(model, glm::radians(this->Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, this->Size);

    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // --- PASSO 1: DESENHO SÓLIDO (Todos fazem isso) ---
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &this->Color[0]);
    glBindVertexArray(VAO_Cubo);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // --- PASSO 2: OUTLINE (Apenas se a flag estiver ativada) ---
    if (this->UseOutline)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f);

        // Cor Preta
        glUniform3f(glGetUniformLocation(shaderID, "objectColor"), 0.0f, 0.0f, 0.0f);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Limpeza
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glBindVertexArray(0);
}
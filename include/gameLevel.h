#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "resourceManager.h"

class GameLevel {
public:
    // Lista de blocos que compõem o cenário
    std::vector<GameObject> Bricks;
    glm::vec3 StartPosition = glm::vec3(0.0f);

    GameLevel() {}

    // Carrega o level de um arquivo (vamos simplificar e carregar de uma matriz por enquanto)
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

    // Desenha todos os blocos do level
    void Draw(unsigned int shaderID, unsigned int cubeVAO);

    // Verifica se o level está completo (para o futuro)
    bool IsCompleted();

private:
    // Função auxiliar para inicializar os blocos baseados nos números
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif
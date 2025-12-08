#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "resourceManager.h"

// ====================================================
// CLASSE GAMELEVEL
// Responsável por armazenar e gerenciar a estrutura de uma fase
// ====================================================

class GameLevel {
public:
    // --- 1. DADOS DO NÍVEL ---
    // Vetor contendo todos os objetos (paredes, chão, moedas, triggers)
    std::vector<GameObject> Bricks;

    // Ponto de spawn inicial do jogador
    glm::vec3 StartPosition;

    // --- 2. CONSTRUTOR ---
    GameLevel() : StartPosition(0.0f) {}

    // --- 3. MÉTODOS PÚBLICOS ---

    // Lê o arquivo de texto e popula o vetor Bricks
    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

    // Renderiza todos os objetos ativos do nível
    void Draw(unsigned int shaderID, unsigned int cubeVAO);

    // Verifica condição de vitória (lógica futura/extensibilidade)
    bool IsCompleted();

private:
    // --- 4. MÉTODOS INTERNOS ---

    // Interpreta a matriz numérica e instancia os GameObjects correspondentes
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif
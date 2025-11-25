#include "gameLevel.h"
#include <fstream>
#include <sstream>
#include <iostream>

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight) {
    this->Bricks.clear();

    // CHÃO PRINCIPAL
    for (int x = -5; x < 50; ++x) {
        // Posição: X varia, Y fixo no chão, Z = 0 (centro)
        glm::vec3 pos((float)x, -2.0f, 0.0f);

        // TAMANHO: 1.0 de largura, 1.0 de altura, e 10.0 DE PROFUNDIDADE (Z)
        // Isso cria uma "estrada" larga
        glm::vec3 size(1.0f, 1.0f, 10.0f);

        glm::vec3 color(0.4f, 0.4f, 0.4f); // Cinza

        GameObject ground(pos, size, color);
        ground.IsSolid = true;
        this->Bricks.push_back(ground);
    }

    // PLATAFORMAS DE TESTE (Mantive tamanho 1x1x1 para serem difíceis em 3D)
    GameObject plat1(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 3.0f), glm::vec3(0.8f, 0.4f, 0.1f));
    plat1.IsSolid = true;
    this->Bricks.push_back(plat1);

    GameObject plat2(glm::vec3(9.0f, 1.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.4f, 0.1f));
    plat2.IsSolid = true;
    this->Bricks.push_back(plat2);
}

void GameLevel::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    for (GameObject& tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(shaderID, cubeVAO);
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) {
    // Calcula tamanho de cada bloco baseado na tela (opcional, ou tamanho fixo 1.0)
    // Vamos usar tamanho fixo 1.0f para manter a sanidade no 3D
    float unit_width = 1.0f;
    float unit_height = 1.0f;

    unsigned int height = (unsigned int)tileData.size();
    unsigned int width = (unsigned int)tileData[0].size();

    // Loop reverso no Y para construir de baixo para cima ou normal de cima para baixo
    // Vamos fazer normal: linha 0 é o topo da tela.
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {

            // Se for 1, cria um bloco sólido
            if (tileData[y][x] == 1) {
                // Posicao no mundo
                // Nota: Y inverte no OpenGL se quisermos coordenadas positivas para cima
                // Mas vamos manter simples: Y negativo vai para baixo.
                glm::vec3 pos(unit_width * x, -unit_height * y, 0.0f); // Z = 0 (Parede de fundo)
                glm::vec3 size(unit_width, unit_height, 1.0f); // Profundidade 1.0

                // Cor: Laranja Tijolo
                glm::vec3 color(0.8f, 0.4f, 0.1f);

                GameObject obj(pos, size, color);
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            }
        }
    }
}
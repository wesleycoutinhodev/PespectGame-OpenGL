#include "gameLevel.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// Configurações de Profundidade
const float TOTAL_DEPTH = 10.0f;
// Define onde começa a parede do fundo. 
const float Z_ANCHOR_BACK = -5.0f;

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight) {
    this->Bricks.clear();
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;

    if (fstream) {
        while (std::getline(fstream, line)) {
            std::vector<unsigned int> row;

            for (char& c : line) {
                if (c == '\r' || c == '\t') continue;

                if (c == ' ') {
                    row.push_back(0);
                    continue;
                }

                // --- MAPEAMENTO DE CARACTERES ---
                if (c == '#') row.push_back(1);      // Chão Sólido
                else if (c == '@') row.push_back(2); // Vitória
                else if (c == 'F') row.push_back(5); // Frente (1/3)
                else if (c == 'M') row.push_back(6); // Meio (2/3)
                else if (c == 'B') row.push_back(7); // Fundo (Tratado como F)
                else if (c == 'X') row.push_back(12); // Caixa
                else if (c == '_') row.push_back(13); // Placa
                else if (c == 'P') row.push_back(9);  // Start

                // NOVOS CÓDIGOS
                else if (c == 'o') row.push_back(8);  // Moeda Ar
                else if (c == '$') row.push_back(50); // COMBO: F + Moeda Fundo
                else if (c == '%') row.push_back(51); // COMBO: M + Moeda Frente

                else if (c == '+') row.push_back(10);
                else if (c == '-') row.push_back(11);
                else row.push_back(0);
            }
            tileData.push_back(row);
        }
        if (tileData.size() > 0) this->init(tileData, levelWidth, levelHeight);
    }
    else {
        std::cout << "ERRO: Nao foi possivel abrir o level: " << file << std::endl;
    }
}

void GameLevel::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    for (GameObject& tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(shaderID, cubeVAO);
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) {
    unsigned int height = (unsigned int)tileData.size();
    unsigned int width = 0;
    for (auto& row : tileData) {
        if (row.size() > width) width = row.size();
    }

    float unit_width = 1.0f;
    float unit_height = 1.0f;

    // --- CÁLCULO DOS CENTROS DAS FATIAS Z ---
    // A profundidade total (10.0) é dividida em 3 partes iguais.
    float sliceDepth = TOTAL_DEPTH / 3.0f; // Aprox 3.33f

    // Calculamos o Z exato do centro de cada fatia
    float zCenterBack = Z_ANCHOR_BACK + (sliceDepth * 0.5f);        // Fatia 1 (Fundo) -> Aprox -3.33
    float zCenterMiddle = Z_ANCHOR_BACK + sliceDepth + (sliceDepth * 0.5f); // Fatia 2 (Meio)  -> 0.00
    float zCenterFront = Z_ANCHOR_BACK + (2.0f * sliceDepth) + (sliceDepth * 0.5f); // Fatia 3 (Frente) -> Aprox +3.33

    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            unsigned int type = 0;
            if (x < tileData[y].size()) type = tileData[y][x];

            if (type == 0) continue;

            glm::vec3 pos(unit_width * x, unit_height * (height - 1 - y), 0.0f);

            glm::vec3 size(unit_width, unit_height, 1.0f);
            glm::vec3 color(1.0f);
            Texture2D texture = ResourceManager::GetTexture("block");

            bool processBlock = false;
            bool isSolid = false;
            float depthPercentage = 1.0f;

            // ==========================================================
            //       LÓGICA DE MOEDAS NO AR (ALINHADAS AO GRID)
            // ==========================================================

            // 'o' -> Moeda Normal (Centro do Meio)
            if (type == 8) {
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterMiddle; // 0.0f

                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }
            // '+' -> Moeda na FRENTE (Centro da Frente)
            else if (type == 10) {
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterFront; // +3.33f

                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 0.8f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }
            // '-' -> Moeda no FUNDO (Centro do Fundo)
            else if (type == 11) {
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterBack; // -3.33f

                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(0.8f, 0.6f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }

            // ==========================================================
            //       LÓGICA DE COMBOS (GRID PERFEITO)
            // ==========================================================

            // '$' -> COMBO FUNDO
            // Parede na Fatia 1 (Fundo) + Moeda na Fatia 2 (Meio)
            else if (type == 50) {
                // 1. Parede F
                float depthF = TOTAL_DEPTH * 0.333f;
                glm::vec3 wallSize(unit_width, unit_height, depthF);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthF / 2.0f); // Centraliza na Fatia 1

                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("block"), glm::vec3(0.8f, 0.4f, 0.1f));
                wall.IsSolid = true;
                this->Bricks.push_back(wall);

                // 2. Moeda (No centro da PRÓXIMA fatia)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterMiddle; // Z = 0.0f

                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }

            // '%' -> COMBO MEIO
            // Parede nas Fatias 1+2 (Meio) + Moeda na Fatia 3 (Frente)
            else if (type == 51) {
                // 1. Parede M
                float depthM = TOTAL_DEPTH * 0.666f;
                glm::vec3 wallSize(unit_width, unit_height, depthM);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthM / 2.0f); // Ocupa Fatias 1 e 2

                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("block"), glm::vec3(0.2f, 0.6f, 0.8f));
                wall.IsSolid = true;
                this->Bricks.push_back(wall);

                // 2. Moeda (No centro da PRÓXIMA fatia)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterFront; // Z = +3.33f

                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }

            // ==========================================================
            //       BLOCOS ESTRUTURAIS NORMAIS
            // ==========================================================
            if (type == 1 || type == 5 || type == 6 || type == 7) {
                processBlock = true;
                isSolid = true;

                if (type == 1) { // #
                    depthPercentage = 1.0f;
                    color = glm::vec3(0.5f, 0.5f, 0.5f);
                    texture = ResourceManager::GetTexture("solid");
                }
                else if (type == 6) { // M
                    depthPercentage = 0.666f;
                    color = glm::vec3(0.2f, 0.6f, 0.8f);
                }
                else if (type == 5 || type == 7) { // F
                    depthPercentage = 0.333f;
                    color = glm::vec3(0.8f, 0.4f, 0.1f);
                }

                size.z = TOTAL_DEPTH * depthPercentage;
                pos.z = Z_ANCHOR_BACK + (size.z / 2.0f);
            }
            else if (type == 12) { // X
                GameObject obj(pos, glm::vec3(0.9f), ResourceManager::GetTexture("crate"), glm::vec3(1.0f));
                obj.IsSolid = true; obj.IsPushable = true; obj.UseOutline = true;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 2) { // @
                GameObject obj(pos, glm::vec3(0.5f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 1.0f, 0.0f));
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 13) { // _
                pos.y -= 0.45f;
                GameObject obj(pos, glm::vec3(0.8f, 0.1f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(0.5f));
                obj.IsPlate = true;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 9) { // P
                this->StartPosition = glm::vec3(pos.x, pos.y, 0.0f);
                continue;
            }

            if (processBlock) {
                GameObject obj(pos, size, texture, color);
                obj.IsSolid = isSolid;
                obj.UseOutline = false;
                this->Bricks.push_back(obj);
            }
        }
    }
}
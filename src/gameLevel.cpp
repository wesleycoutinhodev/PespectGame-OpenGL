#include "gameLevel.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

const float TOTAL_DEPTH = 10.0f;
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

                if (c == ' ') { row.push_back(0); continue; }
                row.push_back((unsigned int)c);
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
    float sliceDepth = TOTAL_DEPTH / 3.0f; // ~3.33f
    float zCenterBack = Z_ANCHOR_BACK + (sliceDepth * 0.5f);
    float zCenterMiddle = Z_ANCHOR_BACK + sliceDepth + (sliceDepth * 0.5f);
    float zCenterFront = Z_ANCHOR_BACK + (2.0f * sliceDepth) + (sliceDepth * 0.5f);

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
            //                PLATAFORMAS MÓVEIS (H / V)
            // ==========================================================
            if (type == 'H') {
                glm::vec3 platPos = pos;
                platPos.z = 0.0f;
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(0.6f, 0.7f, 1.0f));
                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.MoveAxis = glm::vec3(1.0f, 0.0f, 0.0f);
                obj.MoveDistance = 2.0f;
                obj.MoveSpeed = 2.0f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 'V') {
                glm::vec3 platPos = pos;
                platPos.z = 0.0f;
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(0.6f, 0.7f, 1.0f));
                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.MoveAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                obj.MoveDistance = 3.0f;
                obj.MoveSpeed = 2.0f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;
                this->Bricks.push_back(obj);
                continue;
            }
            // ----------------------------------------------------
            // SISTEMA DE LINK (CANAL 1)
            // ----------------------------------------------------

            // '1' = PLACA DE PRESSÃO (TRIGGER ID 1)
            else if (type == '1') {
                pos.y -= 0.45f; // No chão
                GameObject obj(pos, glm::vec3(0.8f, 0.1f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.2f, 0.2f)); // Vermelha

                obj.IsSolid = false;
                obj.IsPlate = true;
                obj.LinkID = 1; // <--- ESTA PLACA MANDA SINAL NO CANAL 1

                this->Bricks.push_back(obj);
                continue;
            }

            // '!' = ELEVADOR VERTICAL (RECEIVER ID 1)
            else if (type == '!') {
                glm::vec3 platPos = pos;
                platPos.z = 0.0f;

                // Textura de metal/grade ficaria legal aqui
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(1.0f, 0.5f, 0.5f)); // Vermelho Claro

                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.IsActivated = false; // <--- COMEÇA PARADO!
                obj.LinkID = 1;          // <--- ESCUTA O CANAL 1

                // Configuração de Movimento
                obj.MoveAxis = glm::vec3(0.0f, 1.0f, 0.0f); // Vertical
                obj.MoveDistance = 4.0f;
                obj.MoveSpeed = 2.0f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;

                this->Bricks.push_back(obj);
                continue;
            }

            // ==========================================================
            //                MOEDAS NO AR (o, +, -)
            // ==========================================================
            else if (type == 'o') { // Moeda Normal (Meio)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterMiddle;
                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }
            else if (type == '+') { // Moeda Frente
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterFront;
                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 0.8f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }
            else if (type == '-') { // Moeda Fundo
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterBack;
                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(0.8f, 0.6f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }

            // ==========================================================
            //                COMBOS ($, %)
            // ==========================================================
            else if (type == '$') { // COMBO FUNDO
                // 1. Parede F
                float depthF = TOTAL_DEPTH * 0.333f;
                glm::vec3 wallSize(unit_width, unit_height, depthF);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthF / 2.0f);
                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("block"), glm::vec3(0.8f, 0.4f, 0.1f));
                wall.IsSolid = true;
                this->Bricks.push_back(wall);

                // 2. Moeda (Centro da Fatia MEIO)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterMiddle;
                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }
            else if (type == '%') { // COMBO MEIO
                // 1. Parede M
                float depthM = TOTAL_DEPTH * 0.666f;
                glm::vec3 wallSize(unit_width, unit_height, depthM);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthM / 2.0f);
                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("block"), glm::vec3(0.2f, 0.6f, 0.8f));
                wall.IsSolid = true;
                this->Bricks.push_back(wall);

                // 2. Moeda (Centro da Fatia FRENTE)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterFront;
                GameObject coin(coinPos, glm::vec3(0.5f, 0.5f, 0.1f), ResourceManager::GetTexture("coin"), glm::vec3(1.0f, 1.0f, 0.0f));
                coin.IsSolid = false; coin.IsCoin = true; coin.UseOutline = true;
                this->Bricks.push_back(coin);
                continue;
            }

            // ==========================================================
            //                BLOCOS ESTRUTURAIS E OBJETOS
            // ==========================================================
            if (type == '#' || type == 'F' || type == 'M' || type == 'B') {
                processBlock = true;
                isSolid = true;

                if (type == '#') { // #
                    depthPercentage = 1.0f;
                    color = glm::vec3(0.5f, 0.5f, 0.5f);
                    texture = ResourceManager::GetTexture("solid");
                }
                else if (type == 'M') { // M
                    depthPercentage = 0.666f;
                    color = glm::vec3(0.2f, 0.6f, 0.8f);
                }
                else if (type == 'F' || type == 'B') { // F
                    depthPercentage = 0.333f;
                    color = glm::vec3(0.8f, 0.4f, 0.1f);
                }

                size.z = TOTAL_DEPTH * depthPercentage;
                pos.z = Z_ANCHOR_BACK + (size.z / 2.0f);
            }
            else if (type == 'X') { // X - Caixa
                GameObject obj(pos, glm::vec3(0.9f), ResourceManager::GetTexture("crate"), glm::vec3(1.0f));
                obj.IsSolid = true; obj.IsPushable = true; obj.UseOutline = true;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == '@') { // @ - Vitória
                GameObject obj(pos, glm::vec3(0.5f), ResourceManager::GetTexture("block"), glm::vec3(1.0f, 1.0f, 0.0f));
                obj.IsSolid = false;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == '_') { // _ - Placa
                pos.y -= 0.45f;
                GameObject obj(pos, glm::vec3(0.8f, 0.1f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(0.5f));
                obj.IsPlate = true;
                this->Bricks.push_back(obj);
                continue;
            }
            // J = JUMP PAD (Trampolim)
            else if (type == 'J') {
                glm::vec3 size(unit_width, unit_height * 0.5f, 1.0f);
                glm::vec3 padPos = pos;
                padPos.z = 0.0f;
                padPos.y -= unit_height * 0.25f;

                GameObject obj(padPos, size, ResourceManager::GetTexture("block"), glm::vec3(0.2f, 1.0f, 0.2f));
                obj.IsSolid = true;    
                obj.IsJumpPad = true; 
                obj.UseOutline = true;

                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 'P') { // P - Player
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
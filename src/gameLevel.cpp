#include "gameLevel.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// ====================================================
// 0. CONSTANTES E DEFINIÇÕES GLOBAIS
// ====================================================

const float TOTAL_DEPTH = 10.0f;
const float Z_ANCHOR_BACK = -5.0f;

// ====================================================
// 1. CARREGAMENTO E I/O (LOAD)
// ====================================================

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight) {
    this->Bricks.clear();

    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;

    if (fstream) {
        while (std::getline(fstream, line)) {
            std::vector<unsigned int> row;
            for (char& c : line) {
                // Ignora caracteres de formatação de arquivo
                if (c == '\r' || c == '\t') continue;

                // Espaço vazio é tratado como 0 (nada)
                if (c == ' ') {
                    row.push_back(0);
                    continue;
                }

                row.push_back((unsigned int)c);
            }
            tileData.push_back(row);
        }

        if (tileData.size() > 0) {
            this->init(tileData, levelWidth, levelHeight);
        }
    }
    else {
        std::cout << "[ERRO CRITICO] Nao foi possivel abrir o arquivo de level: " << file << std::endl;
    }
}

// ====================================================
// 2. RENDERIZAÇÃO (DRAW)
// ====================================================

void GameLevel::Draw(unsigned int shaderID, unsigned int cubeVAO) {
    for (GameObject& tile : this->Bricks) {
        if (!tile.Destroyed) {
            // Bloco try-catch mantido para segurança contra falhas de renderização em objetos específicos
            try {
                tile.Draw(shaderID, cubeVAO);
            }
            catch (...) {
                // Silencia erros de renderização individuais para evitar crash do loop
            }
        }
    }
}

// ====================================================
// 3. INICIALIZAÇÃO LÓGICA (INIT)
// ====================================================

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight) {
    // 1. Calcula dimensões da matriz
    unsigned int height = (unsigned int)tileData.size();
    unsigned int width = 0;
    for (auto& row : tileData) {
        if (row.size() > width) width = row.size();
    }

    float unit_width = 1.0f;
    float unit_height = 1.0f;

    // 2. Cálculo de fatias de profundidade (Z-Slicing)
    // Divide o mundo 3D em 3 camadas de profundidade para jogabilidade 2.5D
    float sliceDepth = TOTAL_DEPTH / 3.0f;
    float zCenterBack = Z_ANCHOR_BACK + (sliceDepth * 0.5f);
    float zCenterMiddle = Z_ANCHOR_BACK + sliceDepth + (sliceDepth * 0.5f);
    float zCenterFront = Z_ANCHOR_BACK + (2.0f * sliceDepth) + (sliceDepth * 0.5f);

    // 3. Loop de parsing da matriz
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {

            // Validação de limites e tipo
            unsigned int type = 0;
            if (x < tileData[y].size()) type = tileData[y][x];
            if (type == 0) continue;

            // Configuração base de posição
            glm::vec3 pos(unit_width * x, unit_height * (height - 1 - y), 0.0f);
            glm::vec3 size(unit_width, unit_height, 1.0f);

            // Variáveis de estado para blocos padrão
            glm::vec3 color(1.0f);
            Texture2D texture = ResourceManager::GetTexture("block");
            bool processBlock = false;
            bool isSolid = false;
            float depthPercentage = 1.0f;

            // --- TIPO A: PLATAFORMAS MÓVEIS (H, V) ---
            if (type == 'H') {
                glm::vec3 platPos = pos; platPos.z = 0.0f;
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(1.0f));

                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.MoveAxis = glm::vec3(1.0f, 0.0f, 0.0f); // Horizontal
                obj.MoveDistance = 6.0f;
                obj.MoveSpeed = 2.5f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;

                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 'V') {
                glm::vec3 platPos = pos; platPos.z = 0.0f;
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(1.0f));

                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.MoveAxis = glm::vec3(0.0f, 1.0f, 0.0f); // Vertical
                obj.MoveDistance = 6.0f;
                obj.MoveSpeed = 2.5f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;

                this->Bricks.push_back(obj);
                continue;
            }

            // --- TIPO B: SISTEMAS LÓGICOS E LINKS ---
            // '1' = Placa de Pressão (Emissor)
            else if (type == '1') {
                pos.y -= 0.45f; // Ajuste para ficar no chão
                GameObject obj(pos, glm::vec3(0.8f, 0.1f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(1.0f));

                obj.IsSolid = false;
                obj.IsPlate = true;
                obj.LinkID = 1; // Canal 1

                this->Bricks.push_back(obj);
                continue;
            }
            // '!' = Elevador Ativável (Receptor)
            else if (type == '!') {
                glm::vec3 platPos = pos; platPos.z = 0.0f;
                GameObject obj(platPos, size, ResourceManager::GetTexture("block"), glm::vec3(1.0f));

                obj.IsSolid = true;
                obj.IsMoving = true;
                obj.IsActivated = false; // Inicia parado
                obj.LinkID = 1;          // Escuta Canal 1

                obj.MoveAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                obj.MoveDistance = 4.0f;
                obj.MoveSpeed = 2.0f;
                obj.InitialX = platPos.x;
                obj.InitialY = platPos.y;

                this->Bricks.push_back(obj);
                continue;
            }

            // --- TIPO C: COLETÁVEIS / MOEDAS (o, +, -) ---
            else if (type == 'o' || type == '+' || type == '-') {
                glm::vec3 coinPos = pos;

                // Define profundidade baseada no caractere
                if (type == 'o') coinPos.z = 0.0f;           // Meio
                else if (type == '+') coinPos.z = 3.0f;      // Frente
                else if (type == '-') coinPos.z = -3.0f;     // Fundo

                GameObject coin(coinPos, glm::vec3(0.5f), ResourceManager::GetTexture("block"), glm::vec3(1.0f));
                coin.IsSolid = false;
                coin.IsCoin = true;
                coin.UseOutline = true;

                Mesh* crystalMesh = ResourceManager::GetModel("crystal");
                if (crystalMesh != nullptr) {
                    coin.ObjectMesh = crystalMesh;
                }
                else {
                    coin.ObjectMesh = nullptr;
                }

                this->Bricks.push_back(coin);
                continue;
            }

            // --- TIPO D: COMBOS ESTRUTURAIS ($, %) ---
            else if (type == '$') {
                // Combo Fundo: Parede atrás + Moeda no meio

                // 1. Parede (Fundo)
                float depthF = TOTAL_DEPTH * 0.333f;
                glm::vec3 wallSize(unit_width, unit_height, depthF);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthF / 2.0f);

                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("solid"), glm::vec3(0.5f, 0.5f, 0.5f));
                wall.IsSolid = true;
                wall.ObjectMesh = nullptr;
                this->Bricks.push_back(wall);

                // 2. Moeda (Meio)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterMiddle;

                GameObject coin(coinPos, glm::vec3(0.5f), ResourceManager::GetTexture("solid"), glm::vec3(1.0f));
                coin.IsSolid = false;
                coin.IsCoin = true;
                coin.UseOutline = true;

                Mesh* crystalMesh = ResourceManager::GetModel("crystal");
                coin.ObjectMesh = (crystalMesh != nullptr) ? crystalMesh : nullptr;

                this->Bricks.push_back(coin);
                continue;
            }
            else if (type == '%') {
                // Combo Meio: Parede no meio + Moeda na frente

                // 1. Parede (Meio)
                float depthM = TOTAL_DEPTH * 0.666f;
                glm::vec3 wallSize(unit_width, unit_height, depthM);
                glm::vec3 wallPos = pos;
                wallPos.z = Z_ANCHOR_BACK + (depthM / 2.0f);

                GameObject wall(wallPos, wallSize, ResourceManager::GetTexture("solid"), glm::vec3(0.5f, 0.5f, 0.5f));
                wall.IsSolid = true;
                wall.ObjectMesh = nullptr;
                this->Bricks.push_back(wall);

                // 2. Moeda (Frente)
                glm::vec3 coinPos = pos;
                coinPos.z = zCenterFront;

                GameObject coin(coinPos, glm::vec3(0.5f), ResourceManager::GetTexture("solid"), glm::vec3(1.0f));
                coin.IsSolid = false;
                coin.IsCoin = true;
                coin.UseOutline = true;

                Mesh* crystalMesh = ResourceManager::GetModel("crystal");
                coin.ObjectMesh = (crystalMesh != nullptr) ? crystalMesh : nullptr;

                this->Bricks.push_back(coin);
                continue;
            }

            // --- TIPO E: BLOCOS ESTRUTURAIS BÁSICOS (#, F, M, B) ---
            if (type == '#' || type == 'F' || type == 'M' || type == 'B') {
                processBlock = true;
                isSolid = true;

                if (type == '#') { // Bloco Completo
                    depthPercentage = 1.0f;
                    color = glm::vec3(0.5f, 0.5f, 0.5f);
                    texture = ResourceManager::GetTexture("solid");
                }
                else if (type == 'M') { // Médio
                    depthPercentage = 0.666f;
                    color = glm::vec3(0.5f, 0.5f, 0.5f);
                    texture = ResourceManager::GetTexture("solid");

                }
                else if (type == 'F' || type == 'B') { // Fino
                    depthPercentage = 0.333f;
                    color = glm::vec3(0.5f, 0.5f, 0.5f);
                    texture = ResourceManager::GetTexture("solid");
                }

                size.z = TOTAL_DEPTH * depthPercentage;
                pos.z = Z_ANCHOR_BACK + (size.z / 2.0f);
            }

            // --- TIPO F: OBJETOS INTERATIVOS E ESPECIAIS ---
            else if (type == 'X') { // Caixa (Pushable)
                GameObject obj(pos, glm::vec3(0.9f), ResourceManager::GetTexture("crate"), glm::vec3(1.0f));
                obj.IsSolid = true;
                obj.IsPushable = true;
                obj.UseOutline = true;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == '@') { // Ponto de Vitória
                GameObject obj(pos, glm::vec3(0.5f), ResourceManager::GetTexture("block"), glm::vec3(1.0f));
                obj.IsSolid = false;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == '_') { // Placa decorativa
                pos.y -= 0.45f;
                GameObject obj(pos, glm::vec3(0.8f, 0.1f, 0.8f), ResourceManager::GetTexture("block"), glm::vec3(0.5f));
                obj.IsPlate = true;
                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 'J') { // Jump Pad (Trampolim)
                glm::vec3 padSize(1.5f, 1.0f, 1.5f);
                pos.y -= 0.30f;
                GameObject obj(pos, padSize, ResourceManager::GetTexture("block"), glm::vec3(1.0f));

                obj.IsSolid = true;
                obj.IsJumpPad = true;

                Mesh* padMesh = ResourceManager::GetModel("pad");
                obj.ObjectMesh = (padMesh != nullptr) ? padMesh : nullptr;

                this->Bricks.push_back(obj);
                continue;
            }
            else if (type == 'P') { // Player Start Position
                this->StartPosition = pos;
                this->StartPosition.y += 0.1f;
                continue;
            }

            // --- PROCESSAMENTO FINAL PARA BLOCOS ESTRUTURAIS ---
            if (processBlock) {
                GameObject obj(pos, size, texture, color);
                obj.IsSolid = isSolid;
                obj.UseOutline = false;
                this->Bricks.push_back(obj);
            }
        }
    }
}
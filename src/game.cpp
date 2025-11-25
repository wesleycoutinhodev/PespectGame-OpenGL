#include "game.h"
#include "resourceManager.h" 
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>


unsigned int cubeVAO, cubeVBO;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Is3DMode(false), MyPlayer(nullptr) {
}

Game::~Game() {
    delete MyPlayer;
}

void Game::Init() {
    // 1. Carregar Shaders (Usando sua classe Shader)
    ResourceManager::LoadShader("shaders/core.vs", "shaders/core.fs", nullptr, "core");

    // 2. Configurar Player
    MyPlayer = new Player(glm::vec3(0.0f, 0.0f, 0.0f));
    MyPlayer->Size = glm::vec3(0.5f, 0.5f, 0.5f);
    MyPlayer->Color = glm::vec3(1.0f, 0.0f, 0.0f);

    // 3. Configurar os Vértices do Cubo (36 vértices para formar 6 faces)
    float vertices[] = {
        // Posições          
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Criar o Level 1
    GameLevel one;
    one.Load("ignored_for_now", this->Width, this->Height);

    this->Levels.push_back(one);
    this->LevelIndex = 0; // Começa no primeiro

    // POSICIONAR PLAYER:
    // Vamos colocar o player numa posição válida do mapa (ex: coluna 2, linha 2)
    //MyPlayer->Position = glm::vec3(2.0f, -2.0f, 0.0f);
    //DEBUG
       
    MyPlayer->Position = glm::vec3(0.0f, 0.0f, 0.0f);
}


bool CheckCollision(GameObject& one, GameObject& two) {
    // Colisão AABB baseada no CENTRO (Center-based)
    // Se a distância entre os centros for menor que a soma das metades dos tamanhos, houve colisão.

    // Eixo X
    bool collisionX = std::abs(one.Position.x - two.Position.x) < (one.Size.x + two.Size.x) / 2.0f;

    // Eixo Y
    bool collisionY = std::abs(one.Position.y - two.Position.y) < (one.Size.y + two.Size.y) / 2.0f;

    // Eixo Z
    bool collisionZ = std::abs(one.Position.z - two.Position.z) < (one.Size.z + two.Size.z) / 2.0f;

    return collisionX && collisionY && collisionZ;
}

// src/game.cpp

void Game::Update(float dt) {
    // 1. Processar Input
    MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode);

    // 2. Aplicar Gravidade
    if (!MyPlayer->IsGrounded) {
        MyPlayer->Velocity.y -= 20.0f * dt;
    }
    else {
        // Se está no chão, aplica uma gravidade minúscula só para manter o IsGrounded true no próximo frame
        // ou zera se quiser "colar" perfeitamente (mas pode causar flutuação em descidas)
        MyPlayer->Velocity.y = -1.0f;
    }

    // --- FÍSICA EIXO X e Z (Horizontal) ---
    // Movemos o player
    MyPlayer->Position.x += MyPlayer->Velocity.x * dt;
    MyPlayer->Position.z += MyPlayer->Velocity.z * dt;

    // Checar colisão horizontal
    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.IsSolid) {
            // Ajuste para colisão centralizada:
            GameObject tempPlayer = *MyPlayer;

            // Diminuímos a altura da caixa de colisão temporária 
            // e subimos ela um pouquinho para não encostar no chão enquanto anda
            tempPlayer.Size.y *= 0.8f;
            tempPlayer.Position.y += 0.1f;

            if (CheckCollision(tempPlayer, brick)) {
                MyPlayer->Position.x -= MyPlayer->Velocity.x * dt;
                MyPlayer->Position.z -= MyPlayer->Velocity.z * dt;
            }
        }
    }

    // --- FÍSICA EIXO Y (Vertical) ---
    MyPlayer->Position.y += MyPlayer->Velocity.y * dt;
    MyPlayer->IsGrounded = false; // Assume que está caindo

    // Checar colisão vertical (Aqui usamos o Player normal, sem levantar o pé)
    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        // ... (Loop vertical no Game::Update) ...
        for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
            if (brick.IsSolid && CheckCollision(*MyPlayer, brick)) {

                // Cálculos auxiliares para facilitar a leitura (Matemática de Centro)
                float playerHalfHeight = MyPlayer->Size.y / 2.0f;
                float brickHalfHeight = brick.Size.y / 2.0f;

                if (MyPlayer->Velocity.y < 0) {
                    // ESTAVA CAINDO -> Pousar no chão

                    // A nova posição Y do player deve ser:
                    // Centro do Bloco + Metade do Bloco (Topo do Bloco) + Metade do Player
                    MyPlayer->Position.y = brick.Position.y + brickHalfHeight + playerHalfHeight;

                    MyPlayer->Velocity.y = 0.0f;
                    MyPlayer->IsGrounded = true;
                }
                else if (MyPlayer->Velocity.y > 0) {
                    // ESTAVA SUBINDO -> Bateu a cabeça

                    // Posição: Centro do Bloco - Metade do Bloco (Base do Bloco) - Metade do Player
                    MyPlayer->Position.y = brick.Position.y - brickHalfHeight - playerHalfHeight;

                    MyPlayer->Velocity.y = 0.0f;
                }
            }
        }
    }

    // Reset se cair do mundo
    if (MyPlayer->Position.y < -15.0f) {
        MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
        MyPlayer->Velocity = glm::vec3(0.0f);
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
   
        static bool cKeyPressed = false;

        if (this->Keys[GLFW_KEY_C] && !cKeyPressed)
        {
            this->Is3DMode = !this->Is3DMode; // Inverte o modo
            cKeyPressed = true;
            std::cout << "Modo 3D: " << (this->Is3DMode ? "ON" : "OFF") << std::endl;
        }

        if (!this->Keys[GLFW_KEY_C])
        {
            cKeyPressed = false;
        }
    }
}

void Game::Render()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // 1. Pegar o Shader
    Shader shader = ResourceManager::GetShader("core");
    shader.use();

    glm::vec3 target = MyPlayer->Position;
    float aspectRatio = (float)this->Width / (float)this->Height;

    glm::mat4 projection;
    glm::mat4 view;

    if (!this->Is3DMode)
    {
        // MODO 2D: Perfeitamente reto
        // projection mantida...
        float zoom = 7.0f;
        projection = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -50.0f, 50.0f);

        view = glm::lookAt(
            glm::vec3(target.x, target.y, 10.0f), // Eye (Câmera)
            glm::vec3(target.x, target.y, 0.0f),  // Center (Alvo)
            glm::vec3(0.0f, 1.0f, 0.0f)           // Up
        );
    }
    else
    {
        // MODO 3D: Terceira Pessoa (Atrás do Player)
        projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

        // CÂMERA ATRÁS:
        // Posição: X = Player - 8 (Atrás), Y = Player + 5 (Acima), Z = Player (Centralizado)
        // Isso assume que o level progride para a DIREITA (X Positivo)
        view = glm::lookAt(
            glm::vec3(target.x - 8.0f, target.y + 5.0f, target.z), // Olho
            glm::vec3(target.x + 5.0f, target.y, target.z),        // Alvo (Olhando à frente do player)
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
    }

    // Enviar matrizes para o shader
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    // 3. Desenhar o Player
    // (A função Draw do GameObject cuida da matriz 'model')
    MyPlayer->Draw(shader.ID, cubeVAO);

    // Desenha o Level Atual
    this->Levels[this->LevelIndex].Draw(shader.ID, cubeVAO);

    // Desenha Player
    MyPlayer->Draw(shader.ID, cubeVAO);
}


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
    // 1. Carregar Shaders
    ResourceManager::LoadShader("shaders/core.vs", "shaders/core.fs", nullptr, "core");

    // 2. CARREGAR TEXTURAS
    ResourceManager::LoadTexture("textures/block.png", false, "block");
    ResourceManager::LoadTexture("textures/block_solid.png", false, "solid");
    ResourceManager::LoadTexture("textures/block_crate.png", true, "crate");
    ResourceManager::LoadTexture("textures/coin.png", true, "coin");

    Shader myShader = ResourceManager::GetShader("core");
    myShader.use();
    myShader.setInt("image", 0);

    // 3. Configurar Player e Level
    GameLevel one;
    one.Load("levels/1.lvl.txt", this->Width, this->Height);
    this->Levels.push_back(one);

    if (one.StartPosition != glm::vec3(0.0f)) {
        MyPlayer = new Player(one.StartPosition);
    }
    else {
        MyPlayer = new Player(glm::vec3(2.0f, -2.0f, 0.0f));
    }

    // 4. Configurar Cubo
    float vertices[] = {
        // Pos                 // Normal           // Tex
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    int stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool CheckCollision(GameObject& one, GameObject& two) {
    bool collisionX = std::abs(one.Position.x - two.Position.x) < (one.Size.x + two.Size.x) / 2.0f;
    bool collisionY = std::abs(one.Position.y - two.Position.y) < (one.Size.y + two.Size.y) / 2.0f;
    bool collisionZ = std::abs(one.Position.z - two.Position.z) < (one.Size.z + two.Size.z) / 2.0f;
    return collisionX && collisionY && collisionZ;
}

void Game::Update(float dt) {
    // 1. Input do Player
    MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode);

    // 2. Gravidade
    if (!MyPlayer->IsGrounded) {
        MyPlayer->Velocity.y -= 20.0f * dt;
    }
    else {
        MyPlayer->Velocity.y = -1.0f;
    }

    // --- FÍSICA HORIZONTAL ---
    MyPlayer->Position.x += MyPlayer->Velocity.x * dt;
    MyPlayer->Position.z += MyPlayer->Velocity.z * dt;

    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.Destroyed) continue;

        if (brick.IsSolid) {
            GameObject tempPlayer = *MyPlayer;
            tempPlayer.Size.y *= 0.8f;
            tempPlayer.Position.y += 0.1f;

            if (CheckCollision(tempPlayer, brick)) {
                if (brick.IsPushable) {
                    brick.Position.x += MyPlayer->Velocity.x * dt;
                    brick.Position.z += MyPlayer->Velocity.z * dt;
                }
                else {
                    MyPlayer->Position.x -= MyPlayer->Velocity.x * dt;
                    MyPlayer->Position.z -= MyPlayer->Velocity.z * dt;
                }
            }
        }
        else if (brick.IsCoin) {
            if (CheckCollision(*MyPlayer, brick)) {
                brick.Destroyed = true;
                std::cout << "MOEDA COLETADA! (+1)" << std::endl;
            }
        }

        if (brick.IsPlate) {
            brick.Active = false;
            brick.Color = glm::vec3(0.5f, 0.5f, 0.5f);
            for (GameObject& box : this->Levels[this->LevelIndex].Bricks) {
                if (box.IsPushable && CheckCollision(box, brick)) {
                    brick.Active = true;
                    brick.Color = glm::vec3(0.0f, 1.0f, 0.0f);
                }
            }
        }
    }

    // --- FÍSICA VERTICAL ---
    MyPlayer->Position.y += MyPlayer->Velocity.y * dt;
    MyPlayer->IsGrounded = false;

    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.Destroyed) continue;

        if (brick.IsSolid && CheckCollision(*MyPlayer, brick)) {
            float playerHalf = MyPlayer->Size.y / 2.0f;
            float brickHalf = brick.Size.y / 2.0f;

            if (MyPlayer->Velocity.y < 0) {
                MyPlayer->Position.y = brick.Position.y + brickHalf + playerHalf;
                MyPlayer->Velocity.y = 0.0f;
                MyPlayer->IsGrounded = true;
            }
            else if (MyPlayer->Velocity.y > 0) {
                MyPlayer->Position.y = brick.Position.y - brickHalf - playerHalf;
                MyPlayer->Velocity.y = 0.0f;
            }
        }
        else if (!brick.IsSolid && !brick.IsCoin && !brick.IsPlate && CheckCollision(*MyPlayer, brick)) {
            std::cout << "VITORIA!" << std::endl;
            MyPlayer->Position = this->Levels[this->LevelIndex].StartPosition;
        }
    }

    if (MyPlayer->Position.y < -15.0f) {
        std::cout << "GAME OVER" << std::endl;
        MyPlayer->Position = this->Levels[this->LevelIndex].StartPosition;
        MyPlayer->Velocity = glm::vec3(0.0f);
    }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        static bool cKeyPressed = false;
        if (this->Keys[GLFW_KEY_C] && !cKeyPressed) {
            this->Is3DMode = !this->Is3DMode;
            cKeyPressed = true;
        }
        if (!this->Keys[GLFW_KEY_C]) cKeyPressed = false;
    }
}

void Game::Render() {
    glEnable(GL_DEPTH_TEST);
    Shader shader = ResourceManager::GetShader("core");
    shader.use();

    glm::vec3 target = MyPlayer->Position;
    float aspectRatio = (float)this->Width / (float)this->Height;
    glm::mat4 projection, view;

    if (!this->Is3DMode) {
        float zoom = 7.0f;
        projection = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -50.0f, 50.0f);
        view = glm::lookAt(glm::vec3(target.x, target.y, 10.0f), glm::vec3(target.x, target.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);
        view = glm::lookAt(target + glm::vec3(-8.0f, 5.0f, 0.0f), target + glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    MyPlayer->Draw(shader.ID, cubeVAO);
    this->Levels[this->LevelIndex].Draw(shader.ID, cubeVAO);
}
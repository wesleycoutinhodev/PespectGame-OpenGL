#include "game.h"
#include "resourceManager.h" 
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath> 
#include <algorithm> 

unsigned int cubeVAO, cubeVBO;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Is3DMode(false), MyPlayer(nullptr),
    Coins(0), CurrentLevel(0), LevelIndex(0)
{
}

Game::~Game() {
    delete MyPlayer;
}

void Game::Init() {
    // 1. Shaders
    ResourceManager::LoadShader("shaders/core.vs", "shaders/core.fs", nullptr, "core");


    // 2. Textures
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "solid");
    ResourceManager::LoadTexture("resources/textures/block_crate.png", true, "crate");
    ResourceManager::LoadTexture("resources/textures/coin.png", true, "coin");

    Shader myShader = ResourceManager::GetShader("core");
    myShader.use();
    myShader.setInt("image", 0);

    // 3. Levels
    this->Levels.clear();
    this->LevelPaths.clear();
    this->LevelPaths.push_back("levels/1.lvl.txt");
    this->LevelPaths.push_back("levels/2.lvl.txt");
    this->LevelPaths.push_back("levels/3.lvl.txt");

    GameLevel currentLevelObj;
    currentLevelObj.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    this->Levels.push_back(currentLevelObj);

    // 4. Player Setup
    if (currentLevelObj.StartPosition != glm::vec3(0.0f)) {
        MyPlayer = new Player(currentLevelObj.StartPosition);
    }
    else {
        MyPlayer = new Player(glm::vec3(2.0f, -2.0f, 0.0f));
    }

    // 5. Cube Geometry (Standard 1x1x1)
    float vertices[] = {
        // Pos                // Normal           // TexCoord
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

    // --- SKYBOX SETUP ---
    ResourceManager::LoadShader("shaders/skybox.vs",
        "shaders/skybox.fs", nullptr, "skybox");

    float skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &this->skyboxVAO);
    glGenBuffers(1, &this->skyboxVBO);
    glBindVertexArray(this->skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Load Cubemap Textures (Verify file names!)
    std::vector<std::string> faces{
        "resources/textures/skybox/px.png", // Right
        "resources/textures/skybox/nx.png", // Left
        "resources/textures/skybox/py.png", // Top
        "resources/textures/skybox/ny.png", // Bottom
        "resources/textures/skybox/pz.png", // Front (Depends on coords)
        "resources/textures/skybox/nz.png"  // Back
    };
    this->skyboxTexture = ResourceManager::LoadCubemap(faces);
}

bool CheckCollision(GameObject& one, GameObject& two) {
    bool collisionX = std::abs(one.Position.x - two.Position.x) < (one.Size.x + two.Size.x) / 2.0f;
    bool collisionY = std::abs(one.Position.y - two.Position.y) < (one.Size.y + two.Size.y) / 2.0f;
    bool collisionZ = std::abs(one.Position.z - two.Position.z) < (one.Size.z + two.Size.z) / 2.0f;
    return collisionX && collisionY && collisionZ;
}

void Game::Update(float dt) {

    // 1. LINK ACTIVATION LOGIC (Plates)
    std::vector<int> activeChannels;

    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.IsPlate && brick.LinkID > 0) {
            brick.Color = glm::vec3(1.0f, 0.2f, 0.2f); // Red (Inactive)
            bool isPressed = false;

            // Check Player
            if (CheckCollision(*MyPlayer, brick)) {
                isPressed = true;
            }
            // Check Pushables
            else {
                for (GameObject& box : this->Levels[this->LevelIndex].Bricks) {
                    if (box.IsPushable && CheckCollision(box, brick)) {
                        isPressed = true;
                        break;
                    }
                }
            }

            if (isPressed) {
                activeChannels.push_back(brick.LinkID);
                brick.Color = glm::vec3(0.2f, 1.0f, 0.2f); // Green (Active)
            }
        }
    }

    // 2. UPDATE ELEVATOR STATE
    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.IsMoving && brick.LinkID > 0) {
            bool shouldMove = false;
            for (int channel : activeChannels) {
                if (channel == brick.LinkID) {
                    shouldMove = true;
                    break;
                }
            }
            brick.IsActivated = shouldMove;
        }
    }

    // 3. MOVE PLATFORMS
    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.IsMoving) {

            glm::vec3 oldPos = brick.Position;

            // --- CASE A: ACTIVE (or Automatic) ---
            if (brick.LinkID == 0 || brick.IsActivated) {
                brick.LocalTime += dt;

                // Cosine Cycle (0 -> 1 -> 0)
                float cycle = (1.0f - cos(brick.LocalTime * brick.MoveSpeed)) * 0.5f;
                float offset = cycle * brick.MoveDistance;

                if (brick.MoveAxis.x > 0) brick.Position.x = brick.InitialX + offset;
                if (brick.MoveAxis.y > 0) brick.Position.y = brick.InitialY + offset;
            }
            // --- CASE B: INACTIVE (Return to Origin) ---
            else {
                float currentDistX = brick.Position.x - brick.InitialX;
                float currentDistY = brick.Position.y - brick.InitialY;
                float currentDist = sqrt(currentDistX * currentDistX + currentDistY * currentDistY);

                if (currentDist > 0.01f) {
                    float returnSpeed = brick.MoveSpeed;

                    if (brick.MoveAxis.x > 0)
                        brick.Position.x -= returnSpeed * dt * (brick.Position.x > brick.InitialX ? 1.0f : -1.0f);

                    if (brick.MoveAxis.y > 0)
                        brick.Position.y -= returnSpeed * dt * (brick.Position.y > brick.InitialY ? 1.0f : -1.0f);

                    // Sync LocalTime to current distance to prevent animation snapping
                    float ratio = currentDist / brick.MoveDistance;
                    if (ratio > 1.0f) ratio = 1.0f;
                    if (ratio < 0.0f) ratio = 0.0f;

                    float angle = acos(1.0f - 2.0f * ratio);
                    float targetAngle = (2.0f * 3.14159f) - angle; // Second half of cycle

                    brick.LocalTime = targetAngle / brick.MoveSpeed;
                }
                else {
                    // Snap to start
                    brick.Position.x = brick.InitialX;
                    brick.Position.y = brick.InitialY;
                    brick.LocalTime = 0.0f;
                }
            }

            // --- CARRY PLAYER ---
            glm::vec3 deltaMovement = brick.Position - oldPos;
            if (glm::length(deltaMovement) > 0.0001f) {
                GameObject tempPlayer = *MyPlayer;
                tempPlayer.Position.y -= 0.1f;

                if (CheckCollision(tempPlayer, brick) && MyPlayer->Position.y > brick.Position.y) {
                    MyPlayer->Position += deltaMovement;
                }
            }
        }
    }

    // 4. INPUT & GRAVITY
    MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode);

    if (!MyPlayer->IsGrounded) {
        MyPlayer->Velocity.y -= 20.0f * dt;
    }
    else {
        MyPlayer->Velocity.y = -1.0f;
    }

    // 5. HORIZONTAL PHYSICS
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
                this->Coins++;
                std::cout << "MOEDA COLETADA!" << std::endl;
            }
        }

        // Simple Plates (Non-linked)
        if (brick.IsPlate && brick.LinkID == 0) {
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

    // 6. VERTICAL PHYSICS
    MyPlayer->Position.y += MyPlayer->Velocity.y * dt;
    MyPlayer->IsGrounded = false;

    for (GameObject& brick : this->Levels[this->LevelIndex].Bricks) {
        if (brick.Destroyed) continue;

        if (brick.IsSolid && CheckCollision(*MyPlayer, brick)) {
            float playerHalf = MyPlayer->Size.y / 2.0f;
            float brickHalf = brick.Size.y / 2.0f;

            if (MyPlayer->Velocity.y < 0) {
                if (brick.IsJumpPad) {
                    MyPlayer->Velocity.y = 25.0f;
                    MyPlayer->JumpCount = 1;
                    MyPlayer->IsGrounded = false;
                    MyPlayer->Position.y = brick.Position.y + brickHalf + playerHalf + 0.1f;
                    std::cout << "BOING!" << std::endl;
                }
                else {
                    MyPlayer->Position.y = brick.Position.y + brickHalf + playerHalf;
                    MyPlayer->Velocity.y = 0.0f;
                    MyPlayer->IsGrounded = true;
                    MyPlayer->JumpCount = 0;
                }
            }
            else if (MyPlayer->Velocity.y > 0) {
                MyPlayer->Position.y = brick.Position.y - brickHalf - playerHalf;
                MyPlayer->Velocity.y = 0.0f;
            }
        }
        else if (!brick.IsSolid && !brick.IsCoin && !brick.IsPlate && CheckCollision(*MyPlayer, brick)) {
            std::cout << "VITORIA!" << std::endl;
            this->NextLevel();
            return;
        }
    }

    // Fall Death
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

        MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode);
    }
}



void Game::Render() {
    glEnable(GL_DEPTH_TEST);
    Shader shader = ResourceManager::GetShader("core");
    shader.use();

    glm::vec3 target = MyPlayer->Position;
    float aspectRatio = (float)this->Width / (float)this->Height;
    glm::mat4 projection, view;

    // --- RENDERIZAÇÃO DO JOGO (CENÁRIO E PLAYER) ---
    if (!this->Is3DMode) {
        // Modo 2D: Projeção Ortográfica (Achatada)
        float zoom = 7.0f;
        projection = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom, -50.0f, 50.0f);
        view = glm::lookAt(glm::vec3(target.x, target.y, 10.0f), glm::vec3(target.x, target.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        // Modo 3D: Projeção Perspectiva (Profundidade)
        projection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);
        view = glm::lookAt(target + glm::vec3(-8.0f, 5.0f, 0.0f), target + glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);

    MyPlayer->Draw(shader.ID, cubeVAO);
    this->Levels[this->LevelIndex].Draw(shader.ID, cubeVAO);

    // --- RENDERIZAÇÃO DO SKYBOX (FUNDO) ---
    // Truque: Mudamos a função de profundidade para desenhar "atrás de tudo"
    glDepthFunc(GL_LEQUAL);

    Shader skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.use();

    // 1. PROJEÇÃO DO CÉU (SEMPRE PERSPECTIVA!)
    // Aqui está a correção: Mesmo que o jogo esteja em 2D, o céu precisa ser 3D para parecer gigante/infinito.
    glm::mat4 skyboxProjection = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f);

    // 2. VISÃO DO CÉU (SEM TRANSLAÇÃO)
    glm::mat4 skyboxView;
    if (!this->Is3DMode) {
        // No 2D: Apenas olhamos para frente (Eixo Z negativo) fixo.
        skyboxView = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        // No 3D: Usamos a mesma rotação da câmera do jogo, mas removemos a posição (translação).
        // Isso faz o céu girar quando a câmera gira, mas não se move quando o player anda.
        skyboxView = glm::mat4(glm::mat3(view));
    }

    skyboxShader.setMat4("view", skyboxView);
    skyboxShader.setMat4("projection", skyboxProjection); // Usa a projeção exclusiva do céu

    glBindVertexArray(this->skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Volta a profundidade ao normal
    glDepthFunc(GL_LESS);
}

void Game::NextLevel() {
    this->CurrentLevel++;

    if (this->CurrentLevel >= this->LevelPaths.size()) {
        this->CurrentLevel = 0;
        std::cout << "GAME CLEARED! Loop." << std::endl;
        this->Coins = 0;
    }

    this->Levels.clear();
    GameLevel next;
    next.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    this->Levels.push_back(next);

    MyPlayer->Velocity = glm::vec3(0.0f);
    MyPlayer->IsGrounded = false;

    if (next.StartPosition != glm::vec3(0.0f))
        MyPlayer->Position = next.StartPosition;
    else
        MyPlayer->Position = glm::vec3(0.0f, 5.0f, 0.0f);
}
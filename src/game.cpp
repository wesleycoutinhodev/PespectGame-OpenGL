#include "game.h"
#include "resourceManager.h" 
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath> 
#include <algorithm> 

// ====================================================
// 0. VARIÁVEIS GLOBAIS E UTILITÁRIOS
// ====================================================

// Variáveis de geometria compartilhada
unsigned int cubeVAO, cubeVBO;

// Função auxiliar de colisão (AABB - Axis Aligned Bounding Box)
bool CheckCollision(GameObject& one, GameObject& two) {
    bool collisionX = std::abs(one.Position.x - two.Position.x) < (one.Size.x + two.Size.x) / 2.0f;
    bool collisionY = std::abs(one.Position.y - two.Position.y) < (one.Size.y + two.Size.y) / 2.0f;
    bool collisionZ = std::abs(one.Position.z - two.Position.z) < (one.Size.z + two.Size.z) / 2.0f;
    return collisionX && collisionY && collisionZ;
}

// ====================================================
// 1. CONSTRUTOR E DESTRUTOR
// ====================================================

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), Is3DMode(false), MyPlayer(nullptr),
    Coins(0), CurrentLevel(0)
{
}

Game::~Game() {
    // Limpeza de recursos de áudio ao encerrar a aplicação
    UnloadMusicStream(this->BgMusic);
    UnloadSound(this->SfxJump);
    UnloadSound(this->SfxCoin);
    CloseAudioDevice();

    delete MyPlayer;
}

// ====================================================
// 2. INICIALIZAÇÃO (INIT)
// ====================================================

void Game::Init() {
    std::cout << "=== INICIANDO GAME ===" << std::endl;

    // --- 1. Carregamento de Shaders ---
    std::cout << "[INFO] Carregando shaders..." << std::endl;
    try {
        ResourceManager::LoadShader("shaders/core.vs", "shaders/core.fs", nullptr, "core");
    }
    catch (...) {
        std::cout << "[ERRO CRITICO] Falha ao carregar shader 'core'!" << std::endl;
        return;
    }

    // --- 2. Carregamento de Texturas ---
    std::cout << "[INFO] Carregando texturas e assets 2D..." << std::endl;
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "solid");
    ResourceManager::LoadTexture("resources/textures/block_crate.png", true, "crate");
    ResourceManager::LoadTexture("resources/textures/win.png", true, "win");

    // Texturas de UI
    ResourceManager::LoadTexture("resources/textures/home_menu.png", true, "home_menu");
    ResourceManager::LoadTexture("resources/textures/player_menu.png", true, "player_menu");
    this->MenuTexture = ResourceManager::GetTexture("home_menu");

    // --- 3. Carregamento de Modelos 3D ---
    std::cout << "[INFO] Carregando modelos 3D..." << std::endl;
    Mesh* lumiMesh = ResourceManager::LoadModel("resources/models/lumi.obj", "resources/models/lumi_texture.png", "lumi");
    ResourceManager::LoadModel("resources/models/crystal.obj", "resources/models/crystal.jpg", "crystal");
    ResourceManager::LoadModel("resources/models/pad.obj", "resources/models/pad.jpg", "pad");

    // --- 4. Configuração Inicial do Shader ---
    Shader myShader = ResourceManager::GetShader("core");
    myShader.use();
    myShader.setInt("image", 0);

    // --- 5. Sistema de Áudio ---
    std::cout << "[INFO] Configurando sistema de audio..." << std::endl;
    InitAudioDevice();

    this->BgMusic = LoadMusicStream("resources/audio/moonlight_theme.mp3");
    this->SfxJump = LoadSound("resources/audio/jump.mp3");
    this->SfxWin = LoadSound("resources/audio/win.mp3");
    this->SfxCoin = LoadSound("resources/audio/coin.mp3");

    SetSoundVolume(this->SfxCoin, 0.3f);
    SetMusicVolume(this->BgMusic, 0.5f);
    SetSoundVolume(this->SfxJump, 0.8f);
    PlayMusicStream(this->BgMusic);

    // Define estado inicial
    State = GAME_MENU;

    // --- 6. Carregamento de Níveis ---
    std::cout << "[INFO] Inicializando levels..." << std::endl;
    this->Levels.clear();
    this->LevelPaths = {
        "levels/1.lvl.txt",
        "levels/2.lvl.txt",
        "levels/3.lvl.txt"
    };

    GameLevel currentLevelObj;
    currentLevelObj.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    this->Levels.push_back(currentLevelObj);

    // --- 7. Setup do Player ---
    std::cout << "[INFO] Criando Player..." << std::endl;
    glm::vec3 startPos = (currentLevelObj.StartPosition != glm::vec3(0.0f))
        ? currentLevelObj.StartPosition
        : glm::vec3(2.0f, -2.0f, 0.0f);

    MyPlayer = new Player(startPos, lumiMesh);
    MyPlayer->Size = glm::vec3(1.5f);

    // --- 8. Geometria do Cubo (VBO/VAO) ---
    // Configura vertices para renderização de cubos padrão
    float vertices[] = {
        // Pos              // Normal           // TexCoords
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); // Position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float))); // TexCoord

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 9. Skybox Setup ---
    ResourceManager::LoadShader("shaders/skybox.vs", "shaders/skybox.fs", nullptr, "skybox");

    float skyboxVertices[] = {
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

    // Carregamento do Cubemap
    std::vector<std::string> faces{
        "resources/textures/skybox/px.png",
        "resources/textures/skybox/nx.png",
        "resources/textures/skybox/py.png",
        "resources/textures/skybox/ny.png",
        "resources/textures/skybox/pz.png",
        "resources/textures/skybox/nz.png"
    };
    this->skyboxTexture = ResourceManager::LoadCubemap(faces);

    std::cout << "=== GAME INICIALIZADO COM SUCESSO ===" << std::endl;
}

// ====================================================
// 3. GAME UPDATE LOOP
// ====================================================

void Game::Update(float dt) {
    // Validação crítica de estado
    if (this->Levels.empty() || !MyPlayer) {
        std::cout << "[ERRO CRITICO] Level ou Player invalido no Update!" << std::endl;
        return;
    }

    UpdateMusicStream(this->BgMusic);

    const unsigned int levelIdx = 0; // Atualmente usamos index 0 pois recarregamos o vetor Levels
    if (levelIdx >= this->Levels.size()) return;

    // --- 1. Lógica de Ativação (Placas de Pressão) ---
    std::vector<int> activeChannels;
    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
        if (brick.IsPlate && brick.LinkID > 0) {
            brick.Color = glm::vec3(1.0f, 0.2f, 0.2f); // Vermelho (Inativo)
            bool isPressed = false;

            if (CheckCollision(*MyPlayer, brick)) {
                isPressed = true;
            }
            else {
                for (GameObject& box : this->Levels[levelIdx].Bricks) {
                    if (box.IsPushable && CheckCollision(box, brick)) {
                        isPressed = true;
                        break;
                    }
                }
            }

            if (isPressed) {
                activeChannels.push_back(brick.LinkID);
                brick.Color = glm::vec3(0.2f, 1.0f, 0.2f); // Verde (Ativo)
            }
        }
    }

    // --- 2. Estado dos Elevadores/Plataformas ---
    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
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

    // --- 3. Movimentação de Plataformas ---
    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
        if (brick.IsMoving) {
            glm::vec3 oldPos = brick.Position;

            if (brick.LinkID == 0 || brick.IsActivated) {
                // Movimento ativo
                brick.LocalTime += dt;
                float cycle = (1.0f - cos(brick.LocalTime * brick.MoveSpeed)) * 0.5f;
                float offset = cycle * brick.MoveDistance;

                if (brick.MoveAxis.x > 0) brick.Position.x = brick.InitialX + offset;
                if (brick.MoveAxis.y > 0) brick.Position.y = brick.InitialY + offset;
            }
            else {
                // Retorno à posição inicial
                float currentDistX = brick.Position.x - brick.InitialX;
                float currentDistY = brick.Position.y - brick.InitialY;
                float currentDist = sqrt(currentDistX * currentDistX + currentDistY * currentDistY);

                if (currentDist > 0.01f) {
                    float returnSpeed = brick.MoveSpeed;
                    if (brick.MoveAxis.x > 0) brick.Position.x -= returnSpeed * dt * (brick.Position.x > brick.InitialX ? 1.0f : -1.0f);
                    if (brick.MoveAxis.y > 0) brick.Position.y -= returnSpeed * dt * (brick.Position.y > brick.InitialY ? 1.0f : -1.0f);

                    // Recalcula LocalTime para suavizar retomada do movimento
                    float ratio = currentDist / brick.MoveDistance;
                    ratio = std::clamp(ratio, 0.0f, 1.0f);
                    float angle = acos(1.0f - 2.0f * ratio);
                    float targetAngle = (2.0f * 3.14159f) - angle;
                    brick.LocalTime = targetAngle / brick.MoveSpeed;
                }
                else {
                    brick.Position.x = brick.InitialX;
                    brick.Position.y = brick.InitialY;
                    brick.LocalTime = 0.0f;
                }
            }

            // Move o jogador junto com a plataforma
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

    // --- 4. Inputs do Jogador e Gravidade ---
    MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode, this->SfxJump);

    if (!MyPlayer->IsGrounded) {
        MyPlayer->Velocity.y -= 20.0f * dt;
    }
    else {
        MyPlayer->Velocity.y = -1.0f;
    }

    // --- 5. Física Horizontal ---
    MyPlayer->Position.x += MyPlayer->Velocity.x * dt;
    MyPlayer->Position.z += MyPlayer->Velocity.z * dt;

    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
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
                PlaySound(this->SfxCoin);
            }
        }

        // Lógica para placas de pressão simples
        if (brick.IsPlate && brick.LinkID == 0) {
            brick.Active = false;
            brick.Color = glm::vec3(0.5f, 0.5f, 0.5f);
            for (GameObject& box : this->Levels[levelIdx].Bricks) {
                if (box.IsPushable && CheckCollision(box, brick)) {
                    brick.Active = true;
                    brick.Color = glm::vec3(0.0f, 1.0f, 0.0f);
                }
            }
        }
    }

    // --- 6. Física Vertical ---
    MyPlayer->Position.y += MyPlayer->Velocity.y * dt;
    MyPlayer->IsGrounded = false;

    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
        if (brick.Destroyed) continue;

        if (brick.IsSolid && CheckCollision(*MyPlayer, brick)) {
            float playerHalf = MyPlayer->Size.y / 2.0f;
            float brickHalf = brick.Size.y / 2.0f;

            if (MyPlayer->Velocity.y < 0) {
                if (brick.IsJumpPad) {
                    // Trampolim
                    MyPlayer->Velocity.y = 25.0f;
                    MyPlayer->JumpCount = 1;
                    MyPlayer->IsGrounded = false;
                    MyPlayer->Position.y = brick.Position.y + brickHalf + playerHalf + 0.1f;
                    PlaySound(this->SfxJump);
                }
                else {
                    // Chão sólido
                    MyPlayer->Position.y = brick.Position.y + brickHalf + playerHalf;
                    MyPlayer->Velocity.y = 0.0f;
                    MyPlayer->IsGrounded = true;
                    MyPlayer->JumpCount = 0;
                }
            }
            else if (MyPlayer->Velocity.y > 0) {
                // Colisão com teto
                MyPlayer->Position.y = brick.Position.y - brickHalf - playerHalf;
                MyPlayer->Velocity.y = 0.0f;
            }
        }
        else if (!brick.IsSolid && !brick.IsCoin && !brick.IsPlate && CheckCollision(*MyPlayer, brick)) {
            // Colisão com o portal/objetivo final
            PlaySound(this->SfxWin);
            this->NextLevel();
            return;
        }
    }

    // --- 7. Condição de Falha (Queda) ---
    if (MyPlayer->Position.y < -15.0f) {
        if (!this->Levels.empty()) {
            MyPlayer->Position = this->Levels[levelIdx].StartPosition;
            MyPlayer->Velocity = glm::vec3(0.0f);
        }
    }
}

void Game::NextLevel() {
    this->CurrentLevel++;

    if (this->CurrentLevel >= this->LevelPaths.size()) {
        this->State = GAME_WIN;
        return;
    }

    std::cout << "[INFO] Avancando para Level " << (this->CurrentLevel + 1) << std::endl;

    // Recarrega o novo level
    this->Levels.clear();
    GameLevel next;
    next.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    this->Levels.push_back(next);

    // Reseta o Player
    MyPlayer->Velocity = glm::vec3(0.0f);
    MyPlayer->IsGrounded = false;
    MyPlayer->JumpCount = 0;

    if (next.StartPosition != glm::vec3(0.0f)) {
        MyPlayer->Position = next.StartPosition;
    }
    else {
        MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
    }
}

// ====================================================
// 4. RENDERIZAÇÃO (RENDER)
// ====================================================

void Game::Render() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader = ResourceManager::GetShader("core");
    shader.use();

    if (!MyPlayer || this->Levels.empty()) return;

    // --- Renderização: Menu Principal ---
    if (this->State == GAME_MENU) {
        glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setBool("isEmissive", true);
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setFloat("glowStrength", 1.0f);

        // Background
        glm::mat4 bgModel = glm::mat4(1.0f);
        bgModel = glm::translate(bgModel, glm::vec3(this->Width / 2.0f, this->Height / 2.0f, 0.0f));
        bgModel = glm::scale(bgModel, glm::vec3(this->Width, this->Height, 1.0f));

        shader.setMat4("model", bgModel);
        shader.setVec3("objectColor", glm::vec3(1.0f));

        glActiveTexture(GL_TEXTURE0);
        this->MenuTexture.Bind();

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // Animação do Personagem no Menu (Flutuando)
        Texture2D playerTexture = ResourceManager::GetTexture("player_menu");
        float time = (float)glfwGetTime();
        float hoverOffset = sin(time * 2.0f) * 20.0f;

        glm::mat4 playerModel = glm::mat4(1.0f);
        playerModel = glm::translate(playerModel, glm::vec3(this->Width / 2.0f, this->Height / 2.0f + hoverOffset, 0.1f));
        playerModel = glm::scale(playerModel, glm::vec3(1920.0f, 1080.0f, 1.0f));

        shader.setMat4("model", playerModel);

        glActiveTexture(GL_TEXTURE0);
        playerTexture.Bind();

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        return;
    }

    // --- Renderização: Tela de Vitória ---
    if (this->State == GAME_WIN) {
        glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setBool("isEmissive", true);
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setFloat("glowStrength", 1.0f);

        glm::mat4 bgModel = glm::mat4(1.0f);
        bgModel = glm::translate(bgModel, glm::vec3(this->Width / 2.0f, this->Height / 2.0f, 0.0f));
        bgModel = glm::scale(bgModel, glm::vec3(this->Width, this->Height, 1.0f));

        shader.setMat4("model", bgModel);
        shader.setVec3("objectColor", glm::vec3(1.0f));

        glActiveTexture(GL_TEXTURE0);
        ResourceManager::GetTexture("win").Bind();

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        return;
    }

    // --- Renderização: Jogo Ativo ---
    if (this->State == GAME_ACTIVE) {
        // Configuração de Iluminação
        if (!this->Is3DMode) {
            shader.setVec3("lightColor", glm::vec3(1.0f, 0.98f, 0.9f));
            shader.setFloat("ambientStrength", 0.7f);
        }
        else {
            shader.setVec3("lightColor", glm::vec3(0.4f, 0.4f, 0.7f));
            shader.setFloat("ambientStrength", 0.3f);
        }

        glm::vec3 target = MyPlayer->Position;
        float aspectRatio = (float)this->Width / (float)this->Height;
        glm::mat4 projection, view;

        // Câmera (Perspectiva vs Ortográfica)
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
        shader.setVec3("lightPos", MyPlayer->Position + glm::vec3(0.0f, 0.5f, 0.0f));

        // Ajuste fino de luz
        if (!this->Is3DMode) {
            shader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.8f));
            shader.setFloat("ambientStrength", 0.6f);
        }
        else {
            shader.setVec3("lightColor", glm::vec3(0.5f, 0.6f, 1.0f));
            shader.setFloat("ambientStrength", 0.1f);
        }

        // Desenhar Player (com efeito emissivo pulsante)
        shader.setBool("isEmissive", true);
        float time = (float)glfwGetTime();
        float pulse = 1.5f + (sin(time * 3.0f) * 0.3f);
        shader.setFloat("glowStrength", pulse);
        MyPlayer->Draw(shader.ID, cubeVAO);

        // Desenhar Cenário
        shader.setBool("isEmissive", false);
        if (!this->Levels.empty()) {
            this->Levels[0].Draw(shader.ID, cubeVAO);
        }

        // Renderização do Skybox
        glDepthFunc(GL_LEQUAL);
        Shader skyboxShader = ResourceManager::GetShader("skybox");
        skyboxShader.use();

        if (!this->Is3DMode) {
            skyboxShader.setVec3("tint", glm::vec3(1.0f, 1.0f, 1.0f));
        }
        else {
            skyboxShader.setVec3("tint", glm::vec3(0.2f, 0.2f, 0.5f));
        }

        float aspectRatioSky = (float)this->Width / (float)this->Height;
        glm::mat4 skyboxProjection = glm::perspective(glm::radians(60.0f), aspectRatioSky, 0.1f, 100.0f);
        glm::mat4 viewMatrix;

        if (!this->Is3DMode) {
            viewMatrix = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            viewMatrix = glm::lookAt(target + glm::vec3(-8.0f, 5.0f, 0.0f), target + glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            viewMatrix = glm::mat4(glm::mat3(viewMatrix)); // Remove translação
        }

        skyboxShader.setMat4("view", viewMatrix);
        skyboxShader.setMat4("projection", skyboxProjection);

        glBindVertexArray(this->skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }
}

// ====================================================
// 5. INPUTS
// ====================================================

void Game::ProcessInput(float dt) {
    if (this->State == GAME_WIN) {
        if (this->Keys[GLFW_KEY_ENTER]) {
            // Reinicia o jogo completo
            this->State = GAME_ACTIVE;
            this->CurrentLevel = 0;
            this->Coins = 0;

            // Recarrega a fase 1
            this->Levels.clear();
            GameLevel one;
            one.Load(this->LevelPaths[0].c_str(), this->Width, this->Height);
            this->Levels.push_back(one);

            // Reseta o Player
            MyPlayer->Position = this->Levels[0].StartPosition;
            MyPlayer->Velocity = glm::vec3(0.0f);

            this->Keys[GLFW_KEY_ENTER] = false;
        }
        return;
    }

    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
        }
    }
    else if (this->State == GAME_ACTIVE) {
        static bool cKeyPressed = false;

        // Toggle da câmera 2D/3D
        if (this->Keys[GLFW_KEY_C] && !cKeyPressed) {
            this->Is3DMode = !this->Is3DMode;
            cKeyPressed = true;
        }
        if (!this->Keys[GLFW_KEY_C]) cKeyPressed = false;
    }
}
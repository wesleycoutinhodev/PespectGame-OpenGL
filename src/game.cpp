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
    // --- LIMPEZA (SÓ ACONTECE QUANDO O JOGO FECHA) ---
    UnloadMusicStream(this->BgMusic);
    UnloadSound(this->SfxJump);
    UnloadSound(this->SfxCoin);
    CloseAudioDevice();

    delete MyPlayer;
}

void Game::Init() {
    std::cout << "=== INICIANDO GAME ===" << std::endl;

    // 1. Shaders
    std::cout << "[1/9] Carregando shaders..." << std::endl;
    try {
        ResourceManager::LoadShader("shaders/core.vs", "shaders/core.fs", nullptr, "core");
        std::cout << "  -> Shader 'core' OK" << std::endl;
    }
    catch (...) {
        std::cout << "  -> ERRO ao carregar shader 'core'!" << std::endl;
        return;
    }

    // 2. Textures
    std::cout << "[2/9] Carregando texturas..." << std::endl;
    ResourceManager::LoadTexture("resources/textures/block.png", false, "block");
    std::cout << "  -> block.png OK" << std::endl;
    ResourceManager::LoadTexture("resources/textures/block_solid.png", false, "solid");
    std::cout << "  -> block_solid.png OK" << std::endl;
    ResourceManager::LoadTexture("resources/textures/block_crate.png", true, "crate");
    std::cout << "  -> block_crate.png OK" << std::endl;
    ResourceManager::LoadTexture("resources/textures/coin.png", true, "coin");
    std::cout << "  -> coin.png OK" << std::endl;

    std::cout << "Carregando modelo 3D do player..." << std::endl;
    Mesh* lumiMesh = ResourceManager::LoadModel(
        "resources/models/lumi.obj",           // Caminho do OBJ
        "resources/models/lumi_texture.png", // Textura
        "lumi"                                  // Nome
    );
    std::cout << "  -> Modelo Lumi carregado OK" << std::endl;

    std::cout << "[3/9] Configurando shader..." << std::endl;
    Shader myShader = ResourceManager::GetShader("core");
    myShader.use();
    myShader.setInt("image", 0);
    std::cout << "  -> Shader configurado OK" << std::endl;

    // --- 1. INICIALIZAR ÁUDIO ---
    std::cout << "[4/9] Inicializando audio..." << std::endl;
    InitAudioDevice();
    std::cout << "  -> Audio device OK" << std::endl;

    // --- 2. CARREGAR SONS ---
    std::cout << "[5/9] Carregando sons..." << std::endl;
    this->BgMusic = LoadMusicStream("resources/audio/moonlight_theme.mp3");
    std::cout << "  -> BgMusic OK" << std::endl;
    this->SfxJump = LoadSound("resources/audio/jump.mp3");
    std::cout << "  -> SfxJump OK" << std::endl;
    this->SfxCoin = LoadSound("resources/audio/coin.mp3");
    std::cout << "  -> SfxCoin OK" << std::endl;
    this->SfxWin = LoadSound("resources/audio/win.mp3");
    std::cout << "  -> SfxWin OK" << std::endl;

    SetMusicVolume(this->BgMusic, 0.5f);
    SetSoundVolume(this->SfxJump, 0.8f);
    PlayMusicStream(this->BgMusic);
    std::cout << "  -> Audio configurado e tocando" << std::endl;

    // --- CARREGAR MENU ---
    std::cout << "[6/9] Carregando menu..." << std::endl;
    ResourceManager::LoadTexture("resources/textures/home_menu.png", true, "home_menu");
    ResourceManager::LoadTexture("resources/textures/player_menu.png", true, "player_menu");
    this->MenuTexture = ResourceManager::GetTexture("home_menu");
    std::cout << "  -> Home OK" << std::endl;
    State = GAME_MENU;

    // 3. Levels
    std::cout << "[7/9] Carregando level..." << std::endl;
    this->Levels.clear();
    this->LevelPaths.clear();
    this->LevelPaths.push_back("levels/1.lvl.txt");
    this->LevelPaths.push_back("levels/2.lvl.txt");
    this->LevelPaths.push_back("levels/3.lvl.txt");

    GameLevel currentLevelObj;
    std::cout << "  -> Carregando level 1..." << std::endl;
    currentLevelObj.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    std::cout << "  -> Level carregado com " << currentLevelObj.Bricks.size() << " objetos" << std::endl;
    this->Levels.push_back(currentLevelObj);

    // 4. Player Setup
    std::cout << "[8/9] Criando player..." << std::endl;
    if (currentLevelObj.StartPosition != glm::vec3(0.0f)) {
        MyPlayer = new Player(currentLevelObj.StartPosition, lumiMesh); // PASSA A MESH
        MyPlayer->Size = glm::vec3(1.5f);
    }
    else {
        MyPlayer = new Player(glm::vec3(2.0f, -2.0f, 0.0f), lumiMesh); // PASSA A MESH
        MyPlayer->Size = glm::vec3(1.5f); 
    }

    // 5. Cube Geometry
    std::cout << "[9/9] Configurando geometria..." << std::endl;
    float vertices[] = {
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
    std::cout << "  -> VAO/VBO criados OK" << std::endl;

    // --- SKYBOX SETUP ---
    std::cout << "Carregando skybox shader..." << std::endl;
    ResourceManager::LoadShader("shaders/skybox.vs", "shaders/skybox.fs", nullptr, "skybox");
    std::cout << "  -> Skybox shader OK" << std::endl;

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
    std::cout << "  -> Skybox VAO OK" << std::endl;

    // Load Cubemap Textures
    std::cout << "Carregando cubemap..." << std::endl;
    std::vector<std::string> faces{
        "resources/textures/skybox/px.png",
        "resources/textures/skybox/nx.png",
        "resources/textures/skybox/py.png",
        "resources/textures/skybox/ny.png",
        "resources/textures/skybox/pz.png",
        "resources/textures/skybox/nz.png"
    };
    this->skyboxTexture = ResourceManager::LoadCubemap(faces);
    std::cout << "  -> Cubemap carregado OK" << std::endl;

    std::cout << "=== GAME INICIALIZADO COM SUCESSO ===" << std::endl;
}

bool CheckCollision(GameObject& one, GameObject& two) {
    bool collisionX = std::abs(one.Position.x - two.Position.x) < (one.Size.x + two.Size.x) / 2.0f;
    bool collisionY = std::abs(one.Position.y - two.Position.y) < (one.Size.y + two.Size.y) / 2.0f;
    bool collisionZ = std::abs(one.Position.z - two.Position.z) < (one.Size.z + two.Size.z) / 2.0f;
    return collisionX && collisionY && collisionZ;
}

void Game::Update(float dt) {
    // Validação crítica
    if (this->Levels.empty() || !MyPlayer) {
        std::cout << "ERRO CRITICO: Level ou Player invalido!" << std::endl;
        return;
    }

    // --- STREAM DE MÚSICA ---
    UpdateMusicStream(this->BgMusic);

    //unsigned int levelIdx = this->CurrentLevel;
    const unsigned int levelIdx = 0;

    if (levelIdx >= this->Levels.size()) {
        std::cout << "ERRO: Indice de level invalido: " << levelIdx << std::endl;
        return;
    }

   

    // 1. LINK ACTIVATION LOGIC (Plates)
    std::vector<int> activeChannels;

    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
        if (brick.IsPlate && brick.LinkID > 0) {
            brick.Color = glm::vec3(1.0f, 0.2f, 0.2f);
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
                brick.Color = glm::vec3(0.2f, 1.0f, 0.2f);
            }
        }
    }

    // 2. UPDATE ELEVATOR STATE
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

    // 3. MOVE PLATFORMS
    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
        if (brick.IsMoving) {
            glm::vec3 oldPos = brick.Position;

            if (brick.LinkID == 0 || brick.IsActivated) {
                brick.LocalTime += dt;
                float cycle = (1.0f - cos(brick.LocalTime * brick.MoveSpeed)) * 0.5f;
                float offset = cycle * brick.MoveDistance;

                if (brick.MoveAxis.x > 0) brick.Position.x = brick.InitialX + offset;
                if (brick.MoveAxis.y > 0) brick.Position.y = brick.InitialY + offset;
            }
            else {
                float currentDistX = brick.Position.x - brick.InitialX;
                float currentDistY = brick.Position.y - brick.InitialY;
                float currentDist = sqrt(currentDistX * currentDistX + currentDistY * currentDistY);

                if (currentDist > 0.01f) {
                    float returnSpeed = brick.MoveSpeed;
                    if (brick.MoveAxis.x > 0) brick.Position.x -= returnSpeed * dt * (brick.Position.x > brick.InitialX ? 1.0f : -1.0f);
                    if (brick.MoveAxis.y > 0) brick.Position.y -= returnSpeed * dt * (brick.Position.y > brick.InitialY ? 1.0f : -1.0f);

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
    MyPlayer->ProcessInput(this->Keys, dt, this->Is3DMode, this->SfxJump);

    if (!MyPlayer->IsGrounded) {
        MyPlayer->Velocity.y -= 20.0f * dt;
    }
    else {
        MyPlayer->Velocity.y = -1.0f;
    }

    // 5. HORIZONTAL PHYSICS
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
                std::cout << "MOEDA COLETADA!" << std::endl;
            }
        }

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

    // 6. VERTICAL PHYSICS
    MyPlayer->Position.y += MyPlayer->Velocity.y * dt;
    MyPlayer->IsGrounded = false;

    for (GameObject& brick : this->Levels[levelIdx].Bricks) {
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
                    PlaySound(this->SfxJump);
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
            PlaySound(this->SfxWin);
            this->NextLevel();
            return;
        }
    }

    if (MyPlayer->Position.y < -15.0f) {
        std::cout << "GAME OVER" << std::endl;
        MyPlayer->Position = this->Levels[levelIdx].StartPosition;
        MyPlayer->Velocity = glm::vec3(0.0f);
    }
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_MENU) {
        if (this->Keys[GLFW_KEY_ENTER]) {
            this->State = GAME_ACTIVE;
            // Opcional: Tocar um som de "Start" aqui
            // PlaySound(this->SfxCoin); 
        }
    }
    else if (this->State == GAME_ACTIVE) {
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader = ResourceManager::GetShader("core");
    shader.use();

    if (!MyPlayer || this->Levels.empty()) {
        std::cout << "ERRO: Impossivel renderizar sem player ou level!" << std::endl;
        return;
    }

    // ====================================================
    // RENDERIZAÇÃO DO MENU
    // ====================================================
    if (this->State == GAME_MENU) {
        // 1. Configura câmera 2D estática (Tela cheia)
        glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setBool("isEmissive", true);
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setFloat("glowStrength", 1.0f);

        // ========================================
        // 2. DESENHA O BACKGROUND (LOGO 16:9)
        // ========================================
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

        // ========================================
        // 3. DESENHA O PERSONAGEM COM LEVITAÇÃO
        // ========================================

        // Carrega a textura do personagem 
        Texture2D playerTexture = ResourceManager::GetTexture("player_menu");

        // Calcula o efeito de levitação (movimento senoidal suave)
        float time = (float)glfwGetTime();
        float hoverOffset = sin(time * 2.0f) * 20.0f;
        float hoverSpeed = 2.0f; // Velocidade da oscilação 
        //float rotation = sin(time * 0.5f) * 5.0f; // Balanço de ±5 graus


        float playerX = this->Width / 2.0f;      // Centro horizontal
        float playerY = this->Height / 2.0f + hoverOffset; // Centro vertical + levitação
        float playerWidth = 1920.0f;              // Largura (ajuste ao tamanho da sprite)
        float playerHeight = 1080.0f;             // Altura (ajuste ao tamanho da sprite)

        glm::mat4 playerModel = glm::mat4(1.0f);
        playerModel = glm::translate(playerModel, glm::vec3(playerX, playerY, 0.1f));
        playerModel = glm::scale(playerModel, glm::vec3(playerWidth, playerHeight, 1.0f));
        //playerModel = glm::rotate(playerModel, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));


        shader.setMat4("model", playerModel);
        shader.setVec3("objectColor", glm::vec3(1.0f));

        glActiveTexture(GL_TEXTURE0);
        playerTexture.Bind();

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        return;
    }

    // --- LÓGICA MOONLIGHT VS DARK SIDE ---
    if (!this->Is3DMode) {
        // MODO 2D (Mundo Iluminado/Planar)
        shader.setVec3("lightColor", glm::vec3(1.0f, 0.98f, 0.9f)); // Luz Quente/Clara
        shader.setFloat("ambientStrength", 0.7f); // Ambiente alto = pouca sombra escura
    }
    else {
        shader.setVec3("lightColor", glm::vec3(0.4f, 0.4f, 0.7f)); // Luz Azulada/Fria
        shader.setFloat("ambientStrength", 0.3f); // Ambiente baixo = sombras escuras
    }
    // -------------------------------------

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

    // =========================================================
    // CONFIGURAÇÃO DA LUZ (LUMI É A FONTE)
    // =========================================================

    // 1. A luz sai de onde o player está (talvez um pouco acima do centro dele)
    shader.setVec3("lightPos", MyPlayer->Position + glm::vec3(0.0f, 0.5f, 0.0f));

    // 2. Definição de Cores (Moonlight vs Dark Side)
    if (!this->Is3DMode) {
        // 2D: Luz Quente, Ambiente Alto (Tudo visível)
        shader.setVec3("lightColor", glm::vec3(1.0f, 0.95f, 0.8f));
        shader.setFloat("ambientStrength", 0.6f);
    }
    else {
        // 3D: Luz Fria, Ambiente Baixo (Escuridão, só vê perto do Lumi)
        shader.setVec3("lightColor", glm::vec3(0.5f, 0.6f, 1.0f));
        shader.setFloat("ambientStrength", 0.1f); // Muito escuro longe dele!
    }

    // =========================================================
    // DESENHAR O PLAYER (BRILHANTE)
    // =========================================================

    // Ativa modo emissivo
    shader.setBool("isEmissive", true);

    // Efeito de "Pulsar" (Seno do tempo faz brilho ir e voltar)
    float time = (float)glfwGetTime();
    float pulse = 1.5f + (sin(time * 3.0f) * 0.3f); // Oscila entre 1.2 e 1.8
    shader.setFloat("glowStrength", pulse);

    MyPlayer->Draw(shader.ID, cubeVAO);

    // =========================================================
    // DESENHAR O CENÁRIO (RECEBE LUZ)
    // =========================================================

    // Desativa modo emissivo (eles são iluminados pelo Lumi)
    shader.setBool("isEmissive", false);

    // Desenha o nível
    this->Levels[this->LevelIndex].Draw(shader.ID, cubeVAO);

    // --- RENDER SKYBOX ---
    glDepthFunc(GL_LEQUAL);
    Shader skyboxShader = ResourceManager::GetShader("skybox");
    skyboxShader.use();

    // =========================================================
    // NOVO: ALTERAÇÃO ATMOSFÉRICA (MOONLIGHT VS DARK SIDE)
    // =========================================================
    if (!this->Is3DMode) {
        // MODO 2D: Céu Normal (Brilhante/Original)
        skyboxShader.setVec3("tint", glm::vec3(1.0f, 1.0f, 1.0f));
    }
    else {
        // MODO 3D: Céu Escuro e Misterioso (Dark Side)
        // O tom azulado (0.5 no Z) ajuda a dar a sensação de "espaço profundo"
        skyboxShader.setVec3("tint", glm::vec3(0.2f, 0.2f, 0.5f));
    }
    // =========================================================

    float aspectRatioSky = (float)this->Width / (float)this->Height;
    glm::mat4 skyboxProjection = glm::perspective(glm::radians(60.0f), aspectRatioSky, 0.1f, 100.0f);

    glm::mat4 viewMatrix;
    if (!this->Is3DMode) {
        viewMatrix = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else {
        viewMatrix = glm::lookAt(target + glm::vec3(-8.0f, 5.0f, 0.0f), target + glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        viewMatrix = glm::mat4(glm::mat3(viewMatrix));
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

void Game::NextLevel() {
    this->CurrentLevel++;

    if (this->CurrentLevel >= this->LevelPaths.size()) {
        this->CurrentLevel = 0;
        std::cout << "GAME CLEARED! Reiniciando do Level 1..." << std::endl;
        this->Coins = 0;
    }

    std::cout << "Carregando Level " << (this->CurrentLevel + 1) << "..." << std::endl;

    // Limpa o level anterior
    this->Levels.clear();

    // Carrega o novo level
    GameLevel next;
    next.Load(this->LevelPaths[this->CurrentLevel].c_str(), this->Width, this->Height);
    this->Levels.push_back(next);

    std::cout << "  -> Level " << (this->CurrentLevel + 1) << " carregado com "
        << next.Bricks.size() << " objetos" << std::endl;

    // Reseta o player
    MyPlayer->Velocity = glm::vec3(0.0f);
    MyPlayer->IsGrounded = false;
    MyPlayer->JumpCount = 0;

    if (next.StartPosition != glm::vec3(0.0f)) {
        MyPlayer->Position = next.StartPosition;
        std::cout << "  -> Player posicionado em: ("
            << next.StartPosition.x << ", "
            << next.StartPosition.y << ", "
            << next.StartPosition.z << ")" << std::endl;
    }
    else {
        MyPlayer->Position = glm::vec3(2.0f, 0.0f, 0.0f);
        std::cout << "  -> Player posicionado em posicao default" << std::endl;
    }
}
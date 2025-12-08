#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <raudio.h>

#include "gameObject.h"
#include "player.h"
#include "gameLevel.h"

// ====================================================
// ENUMERAÇÕES
// ====================================================

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// ====================================================
// CLASSE GAME
// ====================================================

class Game {
public:
    // --- 1. Estado e Configurações ---
    GameState    State;
    bool         Keys[1024];
    unsigned int Width, Height;
    bool         Is3DMode; // Alterna entre 2D (Side Scroller) e 3D (Top-down/Iso)

    // --- 2. Dados de Gameplay ---
    unsigned int Coins;
    unsigned int CurrentLevel;

    // --- 3. Objetos e Níveis ---
    Player* MyPlayer;
    std::vector<GameLevel>   Levels;
    std::vector<std::string> LevelPaths;

    // --- 4. Recursos de Renderização ---
    Texture2D    MenuTexture;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    unsigned int skyboxTexture;

    // --- 5. Sistema de Áudio ---
    Music BgMusic;
    Sound SfxJump;
    Sound SfxCoin;
    Sound SfxWin;

    // ====================================================
    // MÉTODOS PÚBLICOS
    // ====================================================

    // Ciclo de Vida
    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();

    // Game Loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    // Lógica de Controle
    void NextLevel();
};

#endif
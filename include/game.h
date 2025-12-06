#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "gameObject.h"
#include "player.h"
#include "gameLevel.h"
#include <raudio.h>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    GameState State;
    bool Keys[1024]; // Armazena estado das teclas
    unsigned int Width, Height;
    bool Is3DMode;   // A mecânica Dodo Duckie

    unsigned int Coins;           
    unsigned int CurrentLevel;    
    std::vector<std::string> LevelPaths;
    
    std::vector<GameLevel> Levels; // Vetor de levels
    unsigned int LevelIndex;

    unsigned int skyboxVAO = 0;
    unsigned int skyboxVBO = 0;
    unsigned int skyboxTexture = 0;

    // --- VARIÁVEIS DE ÁUDIO ---
    Music BgMusic = {0};    
    Sound SfxJump = {0};
    Sound SfxCoin = {0};
    Sound SfxWin = { 0};
    // --------------------------

    Texture2D MenuTexture;
    

    Game(unsigned int width, unsigned int height);
    ~Game();

    // Inicializa shaders, texturas e levels
    void Init();

    // Loop do jogo
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void NextLevel(); 

    // Lista de objetos do jogo (Paredes, chão, etc)
    std::vector<GameObject> Bricks;
    Player* MyPlayer; // Ponteiro para o jogador
};

#endif
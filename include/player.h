#ifndef PLAYER_H
#define PLAYER_H

#include "gameObject.h"
#include <GLFW/glfw3.h> 

class Player : public GameObject {
public:
    float MoveSpeed;
    float JumpForce;
    glm::vec3 Velocity; // <--- NOVO: Velocidade atual (X, Y, Z)
    bool IsGrounded;    // <--- NOVO: Se está tocando o chão

    Player(glm::vec3 pos);

    // Agora processa input e define a VELOCIDADE desejada, não a posição
    void ProcessInput(bool* keys, float dt, bool is3DMode);
};

#endif
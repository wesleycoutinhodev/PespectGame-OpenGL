#ifndef PLAYER_H
#define PLAYER_H

#include "gameObject.h"
#include <GLFW/glfw3.h>
#include <raudio.h>

class Player : public GameObject {
public:
    float MoveSpeed;
    float JumpForce;
    glm::vec3 Velocity;
    bool IsGrounded;    
    int JumpCount;

    Player(glm::vec3 pos);

    void ProcessInput(bool* keys, float dt, bool is3DMode, Sound jumpSfx);
};

#endif
#ifndef PLAYER_H
#define PLAYER_H

#include "gameObject.h"
#include "mesh.h"
#include <raudio.h>

class Player : public GameObject {
public:
    float MoveSpeed;
    float JumpForce;
    glm::vec3 Velocity;
    bool IsGrounded;
    int JumpCount;
    Mesh* PlayerMesh;

    Player(glm::vec3 pos);
    Player(glm::vec3 pos, Mesh* mesh);

    void ProcessInput(bool* keys, float dt, bool is3DMode, Sound jumpSfx);
    void Draw(unsigned int shaderID, unsigned int cubeVAO) override;
};

#endif
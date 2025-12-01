#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "texture.h" 

class GameObject {
public:
    // Estado do objeto
    glm::vec3 Position, Size, Color;
    float Rotation;
    bool IsSolid;
    bool Destroyed;
    bool UseOutline;
    bool IsPushable;
    bool IsPlate;
    bool IsCoin;
    bool Active;

    // --- VARIÁVEIS PLATAFORMA MÓVEL ---
    bool IsMoving;       
    glm::vec3 MoveAxis;  
    float MoveDistance; 
    float MoveSpeed;     
    float InitialX, InitialY;

    bool IsJumpPad;

    int LinkID;        
    float LocalTime;    
    bool IsActivated;  
    


    // Render state
    Texture2D Sprite;

    GameObject();
    GameObject(glm::vec3 pos, glm::vec3 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f));

    virtual void Draw(unsigned int shaderID, unsigned int cubeVAO);
};

#endif
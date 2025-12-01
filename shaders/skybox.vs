#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos; // No Cubemap, a posição local É a coordenada da textura
    vec4 pos = projection * view * vec4(aPos, 1.0);
    
    // TRUQUE: Força o Z a ser 1.0 (o fundo máximo)
    // xyww faz com que, após a divisão perspectiva (x/w, y/w, z/w), o z seja w/w = 1.0
    gl_Position = pos.xyww; 
}
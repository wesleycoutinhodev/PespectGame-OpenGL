#version 330 core

// ====================================================
// 1. ATRIBUTOS DE ENTRADA (LAYOUTS)
// ====================================================
layout (location = 0) in vec3 aPos;       // Posição do vértice (Model Space)
layout (location = 1) in vec3 aNormal;    // Vetor Normal
layout (location = 2) in vec2 aTexCoords; // Coordenadas de Textura (UV)

// ====================================================
// 2. VARIÁVEIS DE SAÍDA (PARA O FRAGMENT SHADER)
// ====================================================
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

// ====================================================
// 3. UNIFORMS (MATRIZES DE TRANSFORMAÇÃO)
// ====================================================
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// ====================================================
// 4. MAIN
// ====================================================
void main()
{
    // Calcula a posição do fragmento no Espaço de Mundo (World Space)
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Calcula a Matriz Normal para corrigir distorções em escalas não-uniformes
    // (Transposta da Inversa remove a influência da translação e corrige a escala)
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    
    // Repassa coordenadas de textura
    TexCoords = aTexCoords;

    // Projeta o vértice final na tela (Clip Space)
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
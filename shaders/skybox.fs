#version 330 core

// ====================================================
// 1. CONFIGURAÇÃO DE SAÍDA
// ====================================================
out vec4 FragColor;

// ====================================================
// 2. ENTRADAS
// ====================================================
in vec3 TexCoords;

// ====================================================
// 3. UNIFORMS
// ====================================================
uniform samplerCube skybox; // Sampler específico para Cubemaps
uniform vec3 tint;          // Cor de tintura (ex: azulado para noite)

// ====================================================
// 4. MAIN
// ====================================================
void main()
{    
    // Amostra o Cubemap usando o vetor 3D interpolado
    vec4 texColor = texture(skybox, TexCoords);
    
    // Aplica a tintura global ao céu
    FragColor = vec4(texColor.rgb * tint, 1.0);
}
#version 330 core

// ====================================================
// 1. ENTRADAS
// ====================================================
layout (location = 0) in vec3 aPos;

// ====================================================
// 2. SAÍDAS
// ====================================================
out vec3 TexCoords; // Vetor 3D para amostragem do Cubemap

// ====================================================
// 3. UNIFORMS
// ====================================================
uniform mat4 projection;
uniform mat4 view;

// ====================================================
// 4. MAIN
// ====================================================
void main()
{
    // Em um Cubemap, a posição do vértice local é usada como vetor de direção
    TexCoords = aPos;

    // Calcula a posição projetada
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // [OTIMIZAÇÃO DE PROFUNDIDADE]
    // O truque .xyww força o componente Z (profundidade) a ser igual a W.
    // Após a divisão perspectiva (Z / W), o resultado será sempre 1.0.
    // Isso garante que o Skybox seja renderizado no "fundo máximo" possível,
    // permitindo otimizações de Early Depth Testing.
    gl_Position = pos.xyww;
}
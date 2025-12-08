#version 330 core

// ====================================================
// 1. CONFIGURAÇÃO DE SAÍDA
// ====================================================
out vec4 FragColor;

// ====================================================
// 2. ENTRADAS (VINDAS DO VERTEX SHADER)
// ====================================================
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

// ====================================================
// 3. UNIFORMS
// ====================================================

// Material e Textura
uniform sampler2D image;
uniform vec3 objectColor;

// Iluminação (Ambiente e Difusa)
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float ambientStrength;

// Efeitos Especiais (Emissão/Glow)
uniform bool isEmissive;   // Se true, ignora luz e brilha
uniform float glowStrength; // Multiplicador de intensidade do brilho

// ====================================================
// 4. MAIN
// ====================================================
void main()
{
    // Amostragem da textura base
    vec4 texColor = texture(image, TexCoords);

    // Descarte de pixels transparentes (Alpha Clipping)
    if(texColor.a < 0.1)
        discard;

    // --- RAMIFICAÇÃO LÓGICA DE RENDERIZAÇÃO ---
    
    if (isEmissive) {
        // [MODO EMISSIVO]
        // O objeto emite luz própria. Ignora sombreamento.
        // glowStrength > 1.0 cria efeito de "bloom" ou neon.
        vec3 emissive = texColor.rgb * objectColor * glowStrength;
        FragColor = vec4(emissive, 1.0);
    }
    else {
        // [MODO ILUMINADO]
        // O objeto reage à luz ambiente e difusa (Phong simplificado).

        // A. Componente Ambiente (Luz base global)
        vec3 ambient = ambientStrength * lightColor;

        // B. Componente Difusa (Luz direcional baseada na normal)
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // C. Atenuação (Inverse Square Law Approximation)
        // Faz a luz decair conforme a distância do jogador aumenta.
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

        // Aplica a atenuação apenas na luz difusa
        diffuse *= attenuation;

        // D. Combinação Final
        vec3 result = (ambient + diffuse) * objectColor * texColor.rgb;
        FragColor = vec4(result, 1.0);
    }
}
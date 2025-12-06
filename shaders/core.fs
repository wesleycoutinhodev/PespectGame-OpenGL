#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform sampler2D image;

// Controle de Atmosfera
uniform vec3 lightColor;
uniform float ambientStrength;

// --- NOVO: POSIÇÃO DA LUZ E CONTROLE DE EMISSÃO ---
uniform vec3 lightPos;      // Agora a posição da luz vem do C++
uniform bool isEmissive;    // Se for true, o objeto brilha e ignora sombras
uniform float glowStrength; // Força do brilho extra (1.0 = normal, 2.0 = muito forte)

void main()
{
    vec4 texColor = texture(image, TexCoords);
    if(texColor.a < 0.1)
        discard;

    // --- SE FOR O LUMI (EMISSIVO) ---
    if (isEmissive) {
        // Ignora a matemática de luz e sombra.
        // Multiplica por glowStrength para ficar "mais branco que o branco" (efeito neon)
        FragColor = vec4(texColor.rgb * lightColor * glowStrength, 1.0);
    }
    // --- SE FOR CENÁRIO (NORMAL) ---
    else {
        // 1. Ambiente
        vec3 ambient = ambientStrength * lightColor;
      
        // 2. Difusa (Luz pontual vinda do Player)
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos); // Direção da luz até o pixel
        
        // Atenuação (Para a luz ficar forte perto e fraca longe)
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Aplica atenuação na luz difusa (o ambiente continua global)
        diffuse *= attenuation;

        // Combina tudo
        vec3 result = (ambient + diffuse) * objectColor * texColor.rgb;
        FragColor = vec4(result, 1.0);
    }
}
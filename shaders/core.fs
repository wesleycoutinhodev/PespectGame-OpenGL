#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;

void main()
{
    // 1. Configuração da Luz (Vamos simular um "Sol" fixo vindo do topo-direita)
    vec3 lightPos = vec3(10.0, 20.0, 20.0); 
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // 2. Luz Ambiente (Para nada ficar 100% preto)
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
  
    // 3. Luz Difusa (A mágica do 3D)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Calcula o ângulo entre a luz e a face. Se for 0 ou menor, está na sombra.
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    // 4. Resultado Final
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}
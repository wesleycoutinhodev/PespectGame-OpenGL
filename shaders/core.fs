#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 objectColor;
uniform sampler2D image;

void main()
{
    // 1. ILUMINAÇÃO
    vec3 lightPos = vec3(5.0, 20.0, 20.0); 
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;
  
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    // 2. TEXTURA (Lê a imagem real)
    vec4 texColor = texture(image, TexCoords);
    
    // Se a imagem tiver transparência (ex: moeda redonda), recorta o fundo
    if(texColor.a < 0.1)
        discard;

    // 3. RESULTADO FINAL
    // Multiplica a luz * cor do objeto (tint) * cor da textura
    vec3 result = (ambient + diffuse) * objectColor * texColor.rgb;
    
    FragColor = vec4(result, 1.0);
}
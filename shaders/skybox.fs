#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 tint;

void main()
{    
    vec4 texColor = texture(skybox, TexCoords);
    FragColor = vec4(texColor.rgb * tint, 1.0);
}
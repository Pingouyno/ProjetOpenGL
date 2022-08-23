#version 330 core
out vec4 FragColor;

in vec3 cubeTexCoord;

uniform samplerCube cube;

void main()
{    
    FragColor = texture(cube, cubeTexCoord);
}

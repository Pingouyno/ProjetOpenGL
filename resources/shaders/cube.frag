#version 330 core
out vec4 FragColor;

in vec3 cubeTexCoord;
in float brightness;
in float redFactor;

uniform samplerCube cube;

void main()
{    
    FragColor = texture(cube, cubeTexCoord) * brightness;
    if (redFactor != 0.0f)
    {
        FragColor[0] = redFactor;
    }
}

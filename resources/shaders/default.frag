#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;
in vec3 color;
in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
   if (texCoord[0] != 0 || texCoord[1] != 0)
   {
      FragColor = texture(tex0, texCoord);
   }else
   {
      FragColor = vec4(color, 1.0f); 
   }
}


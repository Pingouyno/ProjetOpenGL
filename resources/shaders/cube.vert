
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normPos;
layout (location = 2) in vec2 texMap;

out vec3 cubeTexCoord;

//matrices
uniform mat4 camMatrix;

void main()
{  
   //définit la position de la caméra
   vec4 pos = camMatrix * vec4(aPos, 1.0f);

   //mettre en BACKGROUND si texMap[0] == à 2.0f (regarder classe Cube3D)
   if (texMap[0] == 2.0f)
   {
      gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
   }else
   {
      gl_Position = pos;
   }

   //définit la position normalisée du cube, on ne prend donc pas la position réelle (permet de savoir quelle face du cube on trace)
   cubeTexCoord = vec3(normPos.x, normPos.y, -normPos.z);
}
/*
Cette classe permet de déterminer sur quelle forme 3D clique la souris.

D'habitude, openGL transforme les coordonnées de monde en coordonnées sur l'écran. Cela se fait
par une série de transformations de matrices :
(coordonnées de monde -> vue de la caméra -> projection sur l'écran -> transformation de ratio (vecteur normal) à pixel)

Nous démontons cette chaîne, en utilisant l'inverse de matrices, ainsi, nous pouvons coonnaître l'endroit où la souris frappe.

référence : https://www.youtube.com/watch?v=DLKN0jExRIM&t=482s
*/

#ifndef MOUSE_PICKER_CLASS_H
#define MOUSE_PICKER_CLASS_H

#include"Camera.h"
#include"Shape.h"

class MousePicker
{
    private:
        Camera* camera;

    public:
        vec3 currentRay;
        MousePicker(Camera* camera);
        void update();
        vec3 calculateMouseRay();

        vec2 toRatioCoords(float pixPosX, float pixPosY);
        vec4 toEyeCoords(vec4 clipCoords);
        vec3 toWorldCoords(vec4 eyeCoords);
};
#endif
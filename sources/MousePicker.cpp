#include"../headers/MousePicker.h"

MousePicker::MousePicker(Camera* camera)
{
    this->camera = camera;
}

//met à jour le rayon de la souris
void MousePicker::update()
{
    this->currentRay = calculateMouseRay();
}

//permet d'obtenir un rayon normalisé à partir de la position de la souris
vec3 MousePicker::calculateMouseRay()
{
    //transformer les coordonnées de la souris en des coordonnées de l'écran (ratio écran)
    vec2 normalizedCoords = toRatioCoords(camera->mousePos.x, camera->mousePos.y);
    
    //comvertir le ratio d'écran en coordonnée normalisées (clip)  ici, z = -1.0f car vecteur sort de la caméra
    vec4 clipCoords = vec4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);

    //convertir les coordonnées de clip en coordonnées de caméra (eye)
    vec4 eyeCoords = toEyeCoords(clipCoords);

    //convertir en vecteur normalisé du monde
    vec3 worldCoords = toWorldCoords(eyeCoords);

    return worldCoords;
}

//convertit les positions de pixels en vecteur normalisé (clip), avec un algorithme
vec2 MousePicker::toRatioCoords(float pixPosX, float pixPosY)
{
    float x = (2.0f * pixPosX) / camera->width - 1.0f;
    float y = - ((2.0f * pixPosY) / camera->height - 1.0f);
    return vec2(x, y);
}

//convertit un vecteur sur l'écran (clip) en coordonnées de caméra (projection), avec l'inverse de la Projection Matrix 
vec4 MousePicker::toEyeCoords(vec4 clipCoords)
{
    mat4 invertedProjection = glm::inverse(camera->projectionMatrix);
    vec4 eyeCoords = invertedProjection * clipCoords;
    return vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0);
}

//convertit des coordonnées de caméra (eye) en coordonnées du monde, avec l'inverse de la View Matrix
//permet d'obtenir un rayon qui correspond au vecteur normalisé de la souris
vec3 MousePicker::toWorldCoords(vec4 eyeCoords)
{
    mat4 invertedView = glm::inverse(camera->viewMatrix);
    vec3 worldCoords = vec3(invertedView * eyeCoords);
    return glm::normalize(worldCoords);
}
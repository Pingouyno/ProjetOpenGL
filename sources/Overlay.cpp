#include"../headers/Overlay.h"

//aucun constructeur par défaut car on doit appeler ce constructeur dans les classes enfant
Overlay::Overlay(Camera *camera)
{
    staticShapes = {};
    clickableShapes = {};
    textEntities = {};
    this->camera = camera;
}

void Overlay::render()
{
    for (Quad2D* ptrShape : staticShapes)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }

    for (Quad2D* ptrShape : clickableShapes)
    {
        if (ptrShape->active)
        {
            ptrShape->render();
        }
    }

    for (TextEntity2D* ptrEntity : textEntities)
    {
        if (ptrEntity->active)
        {
            ptrEntity->render();
        }
    }
}

void Overlay::checkCollisions(glm::vec3 &mousePos)
{
    for (Quad2D* ptrShape : clickableShapes)
    {
        if (ptrShape->active && ptrShape->isColliding(mousePos))
        {
            ptrShape->doClickLogic();

            /*on échappe car on ne cliquer qu'un seul icône à la fois.
                de plus, si un des boutons réactive le suivant dans la liste, 
                on peut avoir un comportement non défini (ré-activation des boutons perpétuelle)*/
            break;
        }
    }

    for (TextEntity2D* ptrEntity : textEntities)
    {
        ptrEntity->checkCollisions(mousePos);
    }
}

void Overlay::addStaticShape(Quad2D* shape)
{
    staticShapes.push_back(shape);
}

void Overlay::addClickShape(Quad2D* shape)
{
    clickableShapes.push_back(shape);
}

void Overlay::addTextEntity2D(TextEntity2D* entity)
{
    this->textEntities.push_back(entity);
}

//fonctions virtuelles (À ÊTRE REDÉFINIES)_________________________________________
void Overlay::setupOverlay()
{
    cout << "\n\nERREUR : fonction Overlay::setupOverlay() non redéfinie dans la classe enfant!\n\n";
}
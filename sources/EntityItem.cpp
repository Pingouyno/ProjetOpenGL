#include"EntityItem.h"

const float EntityItem::ITEM_CUBE_SIZE = Block::BLOCK_SIZE / 3.0f;

EntityItem::EntityItem(vec3 pos, Texture* tex) : Entity(pos)
{
    this->behavior = getDefaultClassBehavior();

    this->itemCube = new Cube3D(pos, vec3(ITEM_CUBE_SIZE), tex);
    addCube3D(itemCube);

    this->hitBox = new Cube3D(pos, vec3(ITEM_CUBE_SIZE), Texture::Air);
    this->hitBox->active = false;
    addCube3D(hitBox);
}

void EntityItem::doAnimation()
{
    this->rotate(Shape::ROT_Y, Entity::RADIAN_CIRCLE / 60.0f);
}

void EntityItem::getAttackedBy(Entity* attacker)
{
    cout << "j'ai été détruit!\n";
}
    
function<void(void)> EntityItem::getDefaultClassBehavior()
{
    return [this]()
    {
        //appliquer la gravité
		addVelocity(Entity::FALLING_VELOCITY);
        this->doAnimation();
    };
}
#include"EntityItem.h"

const float EntityItem::ITEM_CUBE_SIZE = Block::BLOCK_SIZE / 4.0f;
const int EntityItem::DESPAWN_TIMER_FRAMES = 60 * 60 * 1; //1 minute
const int EntityItem::PICKUP_TIMER_MILLI = 1000 * 2; //2 secondes

EntityItem::EntityItem(vec3 pos, Texture* tex) : Entity(pos)
{
    this->behavior = getDefaultClassBehavior();

    this->framesUntilDespawn = DESPAWN_TIMER_FRAMES;
    this->itemCube = new Cube3D(pos, vec3(ITEM_CUBE_SIZE), tex);
    addCube3D(itemCube);

    this->hitBox = new Cube3D(pos, vec3(ITEM_CUBE_SIZE), Texture::Air);
    this->hitBox->active = false;
    this->hitBoxDimensions = vec3(ITEM_CUBE_SIZE);
    this->canGetPlacedBlockOn = true;
    addCube3D(hitBox);

    //parce que Entity ne le voit pas
    resetAttackImmuneTimer();
}

int EntityItem::getAttackImmuneTimeConst()
{
    return PICKUP_TIMER_MILLI;
}

void EntityItem::doAnimation()
{
    this->rotate(Shape::ROT_Y, Entity::RADIAN_CIRCLE / 90.0f);
}

void EntityItem::Delete()
{
    delete(this->itemCube);
    delete(this);
}
    
function<void(void)> EntityItem::getDefaultClassBehavior()
{
    return [this]()
    {
        //faire le décompte pour despawn
        framesUntilDespawn--;
        this->doAnimation();
    };
}
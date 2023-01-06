#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<functional> 
#include<cmath>
using namespace std;
#include <random>

#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"Cube3D.h"
#include"Chunk.h"
#include"PlaySound.h"

class Entity
{
    public :  
        const static float RADIAN_CIRCLE;
        const static float JUMP_FALL_ACCELERATION;
        const static vec3 JUMPING_VELOCITY;
        const static vec3 FALLING_VELOCITY;
        const static float DEFAULT_MAX_SPEED;
        const static int ATTACK_COOLDOWN_FRAME;
        const static int DEFAULT_HEALTH;
        const static int DEATH_TIMER_FRAME;

    private:
        mat4 originTransposition;
        vector<Entity*> subEntities;

    protected:
        glm::vec3 pos;
        function<void(void)> behavior;
        vector<Shape*> entityShapes;
        vector<Cube3D*> entityCubes3D;
        //pour se rappeler si un jour on doit delete une entité
        vector<Entity*> referencedEntities;
        
    public:
        int framesUntilattackImmuneEnd;
        //stocke les collisions sur le dernier frame
        vec3 collisionLog;
        float maxSpeed;
        int health;
        bool isDead;
        //important de faire addCube3D(hitbox) pour qu'elle suive l'entité dans les rotations
        Cube3D* hitBox;
        vec3 hitBoxDimensions;
        vec3 velocity;
        //utile pour connaître le contexte (si y==0 est-ce qu'on touche une plateforme ou un toit?). n'a pas été manipulée
        vec3 previousRawVelocity;
        bool active;
        bool isAttackImmune;
        bool canGetPlacedBlockOn;
        Entity(glm::vec3 pos);

        void render();
        void render3DCubes();
        void checkEndOfAttackImmuneTimer();
        void doBehavior();
        void setRed(bool isRed);
        void setBehavior(function<void(void)> behavior);
        void addShape(Shape* ptrShape);
        void addCube3D(Cube3D* ptrCube);
        void addEntity(Entity* entity);
        void moveTo(glm::vec3 newPos);
        void setVelocity(vec3 velocityToAdd);
        void addVelocity(vec3 velocityToAdd);
        void addVelocityCapped(vec3 velocityToAdd);
        void resetVelocity();
        void moveToVelocity();
        vec3 getPotentialNewPos();
        void jump();
        bool wouldBeCollidingEntityVelocity(Entity* otherEntity);
        void attackEntity(Entity* attackedEntity);
        void addReference(Entity* entityToReference);
        void rotate(vec3 axis, float radians);
        void rotateAround(vec3 pos, vec3 axis, float radians);
        void lookAtHorizontal(vec3 targetPos);
        bool isTouchingGround();
        bool isColliding(vec3 pos);
        bool isColliding(Entity* otherEntity);
        bool wouldThenBeCollidingCube(vec3 &testedVelocity, Cube3D* worldCube);
        void reportCollisionWithCubeThen(Cube3D* worldCube);
        vec3 getLocalEquivalent(vec3 axis);
        vec3 getXAxis();
        vec3 getYAxis();
        vec3 getZAxis();
        glm::vec3& getPos();
        float getPos(int i);
        void resetAttackImmuneTimer();

        //fonctions à redéfinir (facultatif)
        virtual void die();
        virtual void removeReference(Entity* entityToDereference);
        virtual void removeReferenceSingle(Entity* entityToDereference);
        virtual int getAttackImmuneFrameConst();
        virtual int getDefaultHealth();
        virtual void initEntity();
        virtual void getAttackedBy(Entity* attacker);
        virtual void Delete();
        virtual function<void(void)> getDeathBehavior();
        virtual void doDeathAnimation();

        //fonctions à redéfinir OBLIGATOIREMENT si on veut utiliser
        virtual void doAnimation();
        virtual void playDeathSound();
        virtual function<void(void)> getDefaultClassBehavior();
};
#endif
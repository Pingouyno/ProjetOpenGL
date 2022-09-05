#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include<functional> 
#include<cmath>
using namespace std;
#include <random>

#include<chrono>
using namespace std::chrono;

#include"Shape.h"
#include"Quad.h"
#include"Cube.h"
#include"Cube3D.h"
#include"Chunk.h"

class Entity
{
    public :  
        const static float RADIAN_CIRCLE;
        const static float JUMP_FALL_ACCELERATION;
        const static vec3 JUMPING_VELOCITY;
        const static vec3 FALLING_VELOCITY;
        const static float DEFAULT_MAX_SPEED;
        const static int ATTACK_COOLDOWN_MILLI;

    private:
        mat4 originTransposition;
        //pour se rappeler si un jour on doit delete une entité
        vector<Entity*> subEntities;

    protected:
        glm::vec3 pos;
        function<void(void)> behavior;
        vector<Shape*> entityShapes;
        vector<Cube3D*> entityCubes3D;
        vector<Entity*> referencedEntities;
        std::chrono::_V2::system_clock::time_point attackImmuneTimeEnd;
        
    public:
        //stocke les collisions sur le dernier frame
        vec3 collisionLog;
        float maxSpeed;
        //important de faire addCube3D(hitbox) pour qu'elle suive l'entité dans les rotations
        Cube3D* hitBox;
        vec3 velocity;
        //utile pour connaître le contexte (si y==0 est-ce qu'on touche une plateforme ou un toit?). n'a pas été manipulée
        vec3 previousRawVelocity;
        bool active;
        bool isAttackImmune;
        Entity(glm::vec3 pos);

        void render();
        void render3DCubes();
        void checkEndOfAttackImmuneTimer();
        void doBehavior();
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
        void removeReference(Entity* entityToDereference);
        void rotate(vec3 axis, float radians);
        void rotateAround(vec3 pos, vec3 axis, float radians);
        void lookAtHorizontal(vec3 targetPos);
        bool isTouchingGround();
        bool isColliding(vec3 pos);
        bool wouldThenBeCollidingCube(vec3 &testedVelocity, Cube3D* worldCube);
        void reportCollisionWithCubeThen(Cube3D* worldCube);
        vec3 getLocalEquivalent(vec3 axis);
        vec3 getXAxis();
        vec3 getYAxis();
        vec3 getZAxis();
        glm::vec3& getPos();
        float getPos(int i);
        void giveAttackImmuneTimer();

        //fonctions à redéfinir (facultatif)
        virtual void doAnimation();
        virtual void getAttackedBy(Entity* attacker);
        
        //fonctions à redéfinir OBLIGATOIREMENT
        virtual function<void(void)> getDefaultClassBehavior();
};
#endif
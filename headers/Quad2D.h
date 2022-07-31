#include"Quad.h"
#include<functional> 
using namespace std;

class Quad2D : public Quad {

    float pixWidth;
    float pixHeight;
    float pixPosX;
    float pixPosY;

    public:
        static vector<Quad2D*> shapesHUD;
        Axis AXIS_2D = Axis::Z;

    //potentiellement remettre public
    public:
        function <void(void)> clickLogic;

    public:
        Quad2D(vector<float> &pos, float pixWidth, float pixheight, Texture* tex, function <void(void)> clickLogic);

        bool isColliding(glm::vec3 &mousePos);
        
        void doClickLogic();
};
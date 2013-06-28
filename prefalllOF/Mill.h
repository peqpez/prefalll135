
#include "ofMain.h"

class Mill {
public:

    void init(float mx, float my, string mpath);
    void draw();
    bool onMill(float ax,float ay);
    void toggleActive();
    bool inArea(float ax, float ay);
    void drawArea();

    float x,y,width,height,scale,vel,n;

    bool active;
    ofImage millImg;
    string path;
    float rot;
    ofPoint p0, p1;

    int valCur, valPre;
    float timeCur, timePre;

};


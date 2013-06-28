
#include "ofMain.h"


class Fish {
public:

    void init(float px, float py, float s, float a, float size);
    void getFish();
    ofPoint calc(float px, float py, float s);
    void wriggle();
    void setTail(float strength, float wave);
    void move();
    float dist( float px, float py );
    float dist( Fish p );
    float angleF( float px, float py );
    float angleF( Fish p );
    void speedChange( float inc );
    void angleChange( float inc );
    void setSpeedLimit( float max, float min );
    void setAngle( float a );
    void turnTo( float ta, float inc );
    void setColor( float c1, float c2, float c3 );
    void copyFish( Fish f );
    bool checkBounds( float turn );
    void align( Fish fp, float angleSpeed, float moveSpeed );
    void cohere( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed  );
    void seperate( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed  );
    void avoid( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed ) ;
    void swarm( float px, float py, float d ) ;
    void scanFlock( Fish flocks[], int flockSize, float cohereR, float avoidR, int hasPredator,bool hasRipple );
    void initColor( float c1, float c2, float c3 );

    float fsize;
    float tailP[2];
    float tailPC[2];
    float tailLength;
    float x, y, angle, speed;
    float maxSpeed, minSpeed;

    float energy; // Energy to wriggle
    float wave; // Tail wave
    int wcount;
    int uturn;
    int boundTime;

    float colour[3], colour0[3];
    float tone;
    bool isBound;
    int inActiveArea;

};




#include "Fish.h"


void Fish::init(float px, float py, float s, float a, float size){

    tailP[0]=tailP[1]=0;
    tailPC[0]=tailPC[1]=0;
    tailLength=3.0;
    energy = 1.0; // Energy to wriggle
    wave = 0; // Tail wave
    wcount = 0;
    uturn = 0;
    boundTime = 0;

    colour[0] = colour[1]=colour[2]=255;
    tone = 0;
    isBound = false;
    inActiveArea=-1;


    tailP[1] = tailLength;
    tailPC[1] = tailLength;

    x = px;
    y = py;
    angle = a;
    speed = s;
    fsize = size;


}


  void Fish::getFish(){
    ofPoint pos1,pos2,pos3;
    ofBeginShape();

    pos1 = calc( 0, -1, fsize );
    ofVertex( pos1.x, pos1.y);

    pos1 = calc( 0.5, -1, fsize );
    pos2 = calc( 1, -0.5, fsize );
    pos3 = calc( 1, 0, fsize );
    ofBezierVertex(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y);

    pos1 = calc( 1, 1, fsize );
    pos2 = calc( tailPC[0], tailPC[1], fsize );
    pos3 = calc( tailP[0], tailP[1], fsize );
    ofBezierVertex(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y);

    pos1 = calc( tailPC[0], tailPC[1], fsize );
    pos2 = calc( -1, 1, fsize );
    pos3 = calc( -1, 0, fsize );
    ofBezierVertex(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y);

    pos1 = calc( -1, -0.5, fsize );
    pos2 = calc( -0.5, -1, fsize );
    pos3 = calc( 0, -1, fsize );
    ofBezierVertex(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y);

    ofEndShape();
  }



    ofPoint Fish::calc( float px, float py, float s ) {
    float a,r;
    ofPoint  pos;
    a = atan2( py, px) + angle+ PI/2;
    r = sqrt( (px*px + py*py) );
    pos.x = x+r*s*cos(a);
    pos.y = y+r*s*sin(a);
    return pos;
  }

  void Fish::wriggle() {

    // Calc energy
    if (energy > 1) {                           // if it has energy
      wcount+=energy*2;                       // tail sine-wave movement
    }

    // Sine-wave oscillation
    if (wcount>120) {
      wcount = 0;
      energy =0;
    }

    wave = sin( wcount*3*PI/180 ); //sine wave
    float strength = energy/5 * tailLength/2; //tail strength

    // Set tail position
    setTail( strength, wave );
    move();
  }


    void Fish::setTail( float strength, float wave ) {
    tailP[0] = strength*wave;
    tailP[1] = tailLength+tailLength/2 - ABS(tailLength/4*strength*wave);
    tailPC[0] = strength*wave*-1;
  }

void Fish::move() {
    x = x+( cos(angle)*speed );
    y = y+( sin(angle)*speed );
  }



  // Find distance
  float Fish::dist( float px, float py ) {
    px -= x;
    py -= y;
    return sqrt( px*px + py*py );
  }

  float Fish::dist( Fish p ) {
    float dx = p.x - x;
    float dy = p.y - y;
    return sqrt( dx*dx + dy*dy );
  }

  // Find angle
  float Fish::angleF( float px, float py ) {
    return atan2( (py-y), (px-x) );
  }

  float Fish::angleF( Fish p ) {
    return atan2( (p.y-y), (p.x-x) );
  }

 void Fish::speedChange( float inc ) {
    speed += inc;
    if (speed<minSpeed) { speed=minSpeed; }
    if (speed>maxSpeed) { speed=maxSpeed; }
  }

  // Direction change
  void Fish::angleChange( float inc ) {
    angle += inc;
  }

  // Set speed limit
  void Fish::setSpeedLimit( float max, float min ) {
    maxSpeed = max;
    minSpeed = min;
  }

  // Set angle
  void Fish::setAngle( float a ) {
    angle = a;
  }



   // Turn towards an angle
  void Fish::turnTo( float ta, float inc ) {

    if (angle < ta) {
      angleChange( inc );
    }
    else {
      angleChange( inc*-1 );
    }
  }


  // Set Color
  void Fish::setColor( float c1, float c2, float c3 ) {
    colour[0] = c1;
    colour[1] = c2;
    colour[2] = c3;
  }

    void Fish::initColor( float c1, float c2, float c3 ) {
    colour0[0] = c1;
    colour0[1] = c2;
    colour0[2] = c3;
  }


  // Copy another fish's angle and pos
  void Fish::copyFish( Fish f ) {
    x = f.x;
    y = f.y;
    angle = f.angle;
    speed = f.speed;
  }

  // Check bounds and U-turn when near bounds
  bool Fish::checkBounds( float turn ) {

    bool inbound = false;

    turn += boundTime/100;

    // Calculate the "buffer area" and turning angle
    float gap = speed * PI/2/turn;
    if (gap > ofGetWidth() /4) {
      gap = ofGetWidth() /4;
      turn = (gap/speed)/PI/2;
    }

    // Which direction to u-turn?
    if ( x-gap < 0 || x+gap > ofGetWidth()  || y-gap < 0 || y+gap > ofGetHeight() ) {

      if (uturn == 0) {

        float temp = angle;
        if (temp < 0) temp += PI*2;

        if ( temp >0 && temp<PI/2 ) {
          uturn = 1;
        }
        else if ( temp >PI/2 && temp<PI ) {
          uturn = -1;
        }
        else if ( temp>PI && temp<PI*3/2 ) {
          uturn = 1;
        }
        else if ( temp>PI*3/2 && temp<PI*2 ) {
          uturn = -1;
        }
        else {
          uturn = 1;
        }

        if (y-gap < 0 || y+gap > ofGetHeight() ) uturn *=-1;
      }

      // Turn
      angleChange( turn*uturn );

      inbound = true;

    }
    else { // when out, clear uturn
      uturn = 0;
      inbound = false;
    }

    x = (x<0) ? 0 : ( (x>ofGetWidth() ) ? ofGetWidth()  : x );
    y = (y<0) ? 0 : ( (y>ofGetHeight() ) ? ofGetHeight() : y );

    isBound = inbound;
    boundTime = (inbound) ? boundTime+1 : 0;

    return inbound;

  }


  // Alignment -- move towards the same direction as the flock within sight
  void Fish::align( Fish fp, float angleSpeed, float moveSpeed ) {

    turnTo( fp.angle, angleSpeed+ ofRandom(0,angleSpeed*3) ); // 0.001

    if ( speed > fp.speed ) {
      speedChange( moveSpeed*(-1-ofRandom(0,1)) ); //0.2  @@@@@@@@@@@@@@@@@@@@
    }
    else {
      speedChange( moveSpeed );
    }

  }


  // Cohesion -- move towards the center of the flock within sight
  void Fish::cohere( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed  ) {

    // get normalised position
    float nx = 0;
    float ny = 0;

    for (int i=0; i<flockSize; i++) {
      nx += flocks[i].x;
      ny += flocks[i].y;
    }

    nx /= flockSize;
    ny /= flockSize;

    turnTo( angleF(nx, ny), angleSpeed+ ofRandom(0,angleSpeed*2) ); //0.001
    speedChange( moveSpeed ); //-0.1

  }


 // Seperation -- moves away from the flock when it's too crowded
  void Fish::seperate( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed  ) {

    // find normalised away angle
    float nA = 0;

    for (int i=0; i<flockSize; i++) {
      nA += (flocks[i].angle+PI);
    }

    nA /= flockSize;
    turnTo( nA, angleSpeed+ ofRandom(0,angleSpeed*2) ); //0.001
    speedChange( moveSpeed ); //0.05
  }



  // Collision aviodance -- moves away quickly when it's too close
  void Fish::avoid( Fish flocks[], int flockSize, float angleSpeed, float moveSpeed ) {

    for (int i=0; i<flockSize; i++) {
      float dA = angleF( flocks[i] ) + PI;

      x = x + cos(dA)*moveSpeed/2;
      y = y + sin(dA)*moveSpeed/2;

      turnTo( dA, angleSpeed+ofRandom(0,angleSpeed) ); //0.005
    }
    speedChange( moveSpeed ); //0.1
  }


    // Attracts towards a point (ie, ripple)
  void Fish::swarm( float px, float py, float d ) {
    float dA = angleF(px, py);

    turnTo( dA, d );
    if (isBound) {
      turnTo( dA, d );
    }
  }




  // Scan for the environment and determines behaviour
  void Fish::scanFlock( Fish flocks[], int flockSize, float cohereR, float avoidR, int hasPredator, bool hasRipple ) {


    Fish Near[flockSize];
    int nCount = 0;
    Fish tooNear[flockSize];
    int tnCount = 0;
    Fish collide[flockSize];
    int cCount = 0;
    Fish nearest;
    bool isNearestNull=true;
    float dist = 99999;

    float tempA = angle;

    // Check boundaries
    bool inbound = (hasPredator>0) ? checkBounds(PI/16) : checkBounds( PI/24);

    for (int i=0; i<flockSize; i++) {

      Fish fp = flocks[i];

      // check nearby fishes
      if (fp.x != x || fp.y!=y) {
        float d = Fish::dist( fp );
        if (d < cohereR ) {
          Near[nCount++] = fp;
          if (dist > d ) {
            dist = d;
            nearest = fp;
            isNearestNull==false;
          }
          if ( d <= avoidR ) {
            tooNear[tnCount++] = fp;
            if ( d <= avoidR/3 ) {
              collide[cCount++] = fp;
            }
          }
        }
      }

      // Calc and make flocking behaviours
      Fish near2[flockSize];
      Fish tooNear2[flockSize];
      Fish collide2[flockSize];

      int j=0;
      for (j=0; j<nCount; j++) {
        near2[j] = Near[j];
      }
      for (j=0; j<tnCount; j++) {
        tooNear2[j] = tooNear[j];
      }
      for (j=0; j<cCount; j++) {
        collide2[j] = collide[j];
      }

      if (!inbound && !hasRipple) {
        if (isNearestNull==false) {
          align( nearest, 0.1*PI/180, 0.2 );
        }
        cohere( near2, nCount, 0.1*PI/180, -0.1 );
      }
      seperate( tooNear2,tnCount, (ofRandom(0,0.1)+0.1)*PI/180, 0.05 );
      avoid( collide2, cCount, (ofRandom(0,0.2)+0.2)*PI/180, 0.1 );
    }

    float diffA = (angle - tempA)*5;
    float c = diffA*180/PI;

    // Wriggle tail
    energy += ABS( c/100 );
    wriggle();

  }




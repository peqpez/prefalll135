#pragma once

//#include "MSACore.h"
//#define MSA_HOST_OPENFRAMEWORKS
#include "MSAFluid.h"
#include "MSATimer.h"


#include "ofMain.h"

#include "Mill.h"
#include "Fish.h"

#include "ofxOsc.h"

#define HOST "localhost"
#define PORT 12345

#define MNUM 9
#define NUM 140


using namespace MSA;

class prefalll : public ofSimpleApp{
public:
	void setup();
	void update();
	void draw();

	void initMills();

	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);

	void windowResized(int w, int h);

	void fadeToColor(float r, float g, float b, float speed);
	void addToFluid(int id, Vec2f pos, Vec2f vel, bool addColor, bool addForce);

	int fluidCellsX;
	bool resizeFluid;
	bool drawFluid;
	bool drawParticles;

	MSA::FluidSolver fluidSolver;
	MSA::FluidDrawerGl fluidDrawer;
	MSA::Vec2f pMouse;

        Fish flock[NUM];
        Fish bigfish1;
        Fish bigfish2;
        Fish bigfish3;

        //ripple
        float rippleX, rippleY;
        bool hasRipple,rm, calibration;
        int hasPredator;


        Mill mills[MNUM];

        ofxOscSender sender;
	ofArduino arduino;

	int i,val0,status;
	float val;
	bool bSetupArduino;
        char* label[9];


};

extern prefalll *myApp;




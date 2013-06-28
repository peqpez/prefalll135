#include "prefalll.h"

using namespace MSA;


//--------------------------------------------------------------
void prefalll::setup() {

    initMills();   //create an object of the class Mill for every mill

    ofHideCursor();
    calibra=false;

    for (int i=0; i<NUM; i++) {  // create the flock of fish
        flock[i].init( ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()), 2, ofRandom(0,PI), ofRandom(2.0,6.0) );
        flock[i].setSpeedLimit( ofRandom(0.3,0.5), 0.7);
        int c=(int)ofRandom(100,200);
        flock[i].setColor( c, 0, 0 );
        flock[i].initColor(c,0,0);
    }


	// setup fluid parameters
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.007).setDeltaT(0.2).setVisc(0.0002).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	fluidDrawer.setDrawMode(MSA::kFluidDrawSpeed);
	fluidCellsX			= 150;
	drawFluid			= true;
	drawParticles		= false;
    resizeFluid			= true;



    //setup the connection with arduino
	status=arduino.connect("COM5",57600); // arduino connected at serial port COM5
    bSetupArduino=false;
    for(i=0;i<9;i++) {
        arduino.sendAnalogPinReporting(i,ARD_ANALOG) ;  //using analog pins 0-9 of arduino mega
    }


    // setup the connection with PD
	sender.setup( HOST, PORT );
    label[0]="/vel0"; label[1]="/vel1"; label[2]="/vel2"; label[3]="/vel3";  // the OSC labels send to PD for each mill
    label[4]="/vel4"; label[5]="/vel5";label[6]="/vel6"; label[7]="/vel7"; label[8]="/vel8";



    ofSetFrameRate(60);
	ofBackground(0, 0, 0);
	ofSetVerticalSync(false);
    windowResized(ofGetWidth(), ofGetHeight());
	pMouse = getWindowCenter();
    ofEnableAlphaBlending();
	ofSetBackgroundAuto(false);
}





void prefalll::update(){

    //update fluids
	if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}

    fluidSolver.update();


    //update arduino
	arduino.update();

    //iterration for the 8 mills (8 arduino PINs)
     for(i=0;i<9;i++){
        val0=arduino.getAnalog(i);   //Reading data from Arduino's Analog PIN i
        mills[i].valPre=mills[i].valCur; //mills[i].valPre is the PREVIOUS VALUE that arduino sent for MIIL i
        mills[i].valCur=val0;           // mills[i].valCur is the CURRENT VALUE that arduino sent for MIIL i
        mills[i].timeCur=ofGetElapsedTimeMillis(); //mills[i].timecCur is the CURRENT TIME


        //Black stripes give values between 0-20, white stripes give values between 200-500 (depending on the distance)
        //If the difference between the current value and the previous is bigger than 100, the mill has been rotated
        if ((abs(mills[i].valCur-mills[i].valPre)>40)||((abs(mills[i].valCur-mills[i].valPre)>20)&&(i==3))){  //The mill has been rotated

            //Calculating the time that has passed between the reception of the previous and current value.
            //with this data we can calculate the velocity
            mills[i].vel=mills[i].timeCur- mills[i].timePre;
            //Mapping values for fluids (FLUIDS 0 - 1 )
            mills[i].vel=ofMap( mills[i].vel, 0,1500,1,0);
            mills[i].timePre=mills[i].timeCur; //keep the current time, to use it as previous in the next iteration
            mills[i].active=(mills[i].vel>0.1); //SETING A THRESHOLD FOR MILL'S ACTIVATION
        }


        //if the time difference is bigger than 1sec we consider the mill stopped
        if (abs(mills[i].timeCur-mills[i].timePre)>1000){
            mills[i].timePre=mills[i].timeCur-1000;
            mills[i].vel=0;
        }


        //Send velocity to PD
        float pd; //the value send to PD
        pd = ofMap( mills[i].vel, 1,0,50,1000); //mapping values for PD  (50 - 1000)
        // Sending an OSC message with mill's velocity to PORT. The label is /veli for MILLi
        ofxOscMessage m;
        m.setAddress( label[i] );
        m.addFloatArg( pd );
        sender.sendMessage( m );

		}

        arduino.setUseDelay(true);

}



void prefalll::draw(){

    //Draw Fluids
	if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
	} else {
		if(getElapsedFrames()%5==0) fadeToColor( 0, 0, 0, 0.1f );
	}

    ofNoFill();

    //Draw flock of fish
    for (int i=0; i<NUM; i++) {
        for(int j=0; j<MNUM; j++)
            //if the fish is in the area of a rotating mill change fish color and velocity
            if ((mills[j].active && mills[j].inArea(flock[i].x,flock[i].y))) {
                flock[i].inActiveArea=j;
                flock[i].colour[0]=flock[i].colour0[0]*mills[j].vel*2.5;
                flock[i].colour[1]=100;
                ofSetColor(flock[i].colour[0], flock[i].colour[1], flock[i].colour[2]);
                ofFill();
                flock[i].swarm( mills[j].x, mills[j].y, PI/mills[j].n );
            }

        rm=(flock[i].inActiveArea<0)?false:true;
        //if the fish left an active area return to normal color-velocity
        if(!rm){
            flock[i].colour[0]=flock[i].colour0[0];
            flock[i].colour[1]=flock[i].colour0[1];
            ofSetColor(flock[i].colour[0], flock[i].colour[1], flock[i].colour[2]);
            ofFill();
        }
        flock[i].scanFlock( flock,NUM, 200, 50,0, false );
        flock[i].move();
        flock[i].getFish();
        flock[i].inActiveArea=-1;

    }

    ofSetColor(255,255,255);


    // Add fluid around a rotating mill
    for(int m=0;m<MNUM;m++) {
        mills[m].draw();
        if(mills[m].active){ //if a mill is rotating create a fluid around a the mill
            float dx=ofRandom(0,70);
            float dy=ofRandom(0,70);
            Vec2f millPos = Vec2f(mills[m].x+dx, mills[m].y+dy);
            Vec2f millNorm = Vec2f( millPos) / getWindowSize();

            int sgnx=(int)ofRandom(-100,100);
            sgnx=(sgnx>0)?1:-1;

            int sgny=(int)ofRandom(-100,100);
            sgny=(sgny>0)?1:-1;

            float dvelx=mills[m].vel/1.4*sgnx;
            float dvely=mills[m].vel/1.4*sgny;
            Vec2f millVel = Vec2f(dvelx,dvely);
            addToFluid(m,millNorm, millVel, true, true);
        }
    }

    if (calibra) { //if calibration mode is selected(key'c') show the position of the mills
        //draw a circle at the position of each mill to help calibration
        ofColor(255,0,0);
        ofCircle( 162,177,15);
        ofCircle( 265,177,15);
        ofCircle( 367,177,15);

        ofCircle( 162,375,15);
        ofCircle( 266,375,15);
        ofCircle( 367,375,15);

        ofCircle( 162,591,15);
        ofCircle( 266,591,15);
        ofCircle( 367,591,15);

    }
}

void prefalll::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}


// add force and dye to fluid
void prefalll::addToFluid( int id, Vec2f pos, Vec2f vel, bool addColor, bool addForce ) {
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = constrain(pos.x, 0.0f, 1.0f);
		pos.y = constrain(pos.y, 0.0f, 1.0f);

        const float colorMult = 100;
        const float velocityMult = 30;

        int index = fluidSolver.getIndexForPos(pos);

		if(addColor) {
			Color drawColor( CM_HSV, 0.6, 1, 2 );
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
		}

		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);

    }
}


void prefalll::windowResized(int w, int h) {}


void prefalll::keyPressed  (int key){
    switch(key) {
        case '0':
         for (int i=0; i<NUM; i++) {
            flock[i].init( ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()), 2, ofRandom(0,PI), ofRandom(2.0,6.0) );
            flock[i].setSpeedLimit( ofRandom(0.3,0.5), 0.7);
            int c=(int)ofRandom(100,200);
            flock[i].setColor( c, 0, 0 );
            flock[i].initColor(c,0,0);

        }break;
		case '1':
			fluidDrawer.setDrawMode(MSA::kFluidDrawColor);
			break;

		case '2':
			fluidDrawer.setDrawMode(MSA::kFluidDrawMotion);
			break;

		case '3':
			fluidDrawer.setDrawMode(MSA::kFluidDrawSpeed);
			break;

		case '4':
			fluidDrawer.setDrawMode(MSA::kFluidDrawVectors);
			break;

		case 'd':
			drawFluid ^= true;
			break;

		case 'p':
			drawParticles ^= true;
			break;

		case 'f':
			ofToggleFullscreen();
			break;

		case 'r':
			fluidSolver.reset();
			break;


			case 'c':
			calibra=!calibra;
			break;

			case 'C':
			calibra=!calibra;
			break;

		case 'b': {
			Timer timer;
			const int ITERS = 3000;
			timer.start();
			for( int i = 0; i < ITERS; ++i ) fluidSolver.update();
			timer.stop();
			//cout << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
		}
			break;

    }
}


//--------------------------------------------------------------
void prefalll::mouseMoved(int x, int y ){

}

void prefalll::mouseDragged(int x, int y, int button) {

}

void prefalll::mousePressed(int x, int y, int button){


    for(int m=0; m<MNUM; m++) {
        if(mills[m].onMill(mouseX,mouseY)) {
            if (button==0) {if(mills[m].vel<1) mills[m].vel+=0.1; }
            else if (button==2) {if(mills[m].vel>0 )mills[m].vel-=0.1;}
            if (mills[m].vel>0.4) mills[m].active=true; else mills[m].active=false;
        }
    }


}

void prefalll::initMills(){ //create 8 mill objects and determinate positions

    float distH=ofGetWidth()/3;
    float distH2=ofGetWidth()/2;
    float distV=ofGetHeight()/3;


    mills[0].init(162,177,"1.png");
    mills[1].init(265,177,"1.png");
    mills[2].init(367,177,"1.png");


     mills[3].init(162,375,"1.png");
    mills[4].init(265,375,"1.png");
    mills[8].init(367,375,"1.png");

    mills[5].init(162,591,"1.png");
    mills[6].init(265,591,"1.png");
    mills[7].init(367,591,"1.png");


    mills[0].p0=ofPoint(0,0); mills[0].p1=ofPoint(distH,distV);
    mills[1].p0=ofPoint(distH,0); mills[1].p1=ofPoint(2*distH,distV);
    mills[2].p0=ofPoint(2*distH,0); mills[2].p1=ofPoint(3*distH,distV);

    mills[3].p0=ofPoint(0,distV); mills[3].p1=ofPoint(distH,2*distV);
    mills[4].p0=ofPoint(distH,distV); mills[4].p1=ofPoint(2*distH,2*distV);
    mills[8].p0=ofPoint(2*distH,distV); mills[8].p1=ofPoint(3*distH,2*distV);

    mills[5].p0=ofPoint(0,2*distV); mills[5].p1=ofPoint(distH,3*distV);
    mills[6].p0=ofPoint(distH,2*distV); mills[6].p1=ofPoint(2*distH,3*distV);
    mills[7].p0=ofPoint(2*distH,2*distV); mills[7].p1=ofPoint(3*distH,3*distV);


}

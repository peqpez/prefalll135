
#include "prefalll.h"

prefalll *myApp;
int main( ){
	ofSetupOpenGL(544, 800, OF_WINDOW);			// <-------- setup the GL context
	ofSetWindowPosition(300,0);
	myApp = new prefalll;
	ofRunApp(myApp);
}

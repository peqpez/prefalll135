#include "ofMain.h"
#include "Mill.h"


void Mill::init(float mx, float my, string mpath){

    x=mx;
    y=my;
    path=mpath;
    millImg.loadImage(path);
    millImg.draw(x,y);
    active=false;
    vel=0.3;
    rot=0;
    width=millImg.getWidth();
    height=millImg.getHeight();
    scale=1.6;
    n=4.0;
    valPre=0;
    timePre=0;
    valCur=0;
    valPre=0;

}


void Mill::draw(){
    n=4/vel;
    ofEnableAlphaBlending();
    ofFill();
    if(active){
        ofPushMatrix();
        rot+=10;
        ofTranslate(x+width*scale/2,y+height*scale/2,0);
        ofRotateZ(rot*vel);
        millImg.draw(0-width*scale/2,0-height*scale/2, width*scale,height*scale);
        ofPopMatrix();
        
    }
    else {
        millImg.draw(x,y,width*scale,height*scale);
    }
    ofDisableAlphaBlending();

}


bool Mill::onMill(float ax,float ay){
    return ((ax>x)&&(ax<x+width*scale)&&(ay>y)&&(ay<y+height*scale));

}

void Mill::toggleActive(){
    active=!active;
    }

bool Mill::inArea(float ax, float ay){
    return ((ax>p0.x)&&(ax<p1.x)&&(ay>p0.y)&&(ay<p1.y));
    }

void Mill::drawArea(){
    ofNoFill();
    ofRect(p0.x,p0.y, p1.x-p0.x,p1.y-p0.y);
    }


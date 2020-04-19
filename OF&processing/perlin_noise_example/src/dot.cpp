//
//  dot.cpp
//  perlin_noise_example
//
//  Created by Waihin Chan on 2020/4/16.
//

#include "dot.hpp"
#include <stdio.h>
dot::dot(ofVec2f ran_init_pos , float ran_max_speed,float _m){
    m = _m;
    pos = ran_init_pos;
    maxspeed = ran_max_speed;
    speed*=0;
    acc*= 0;
    prepos*=0;

    
}

void dot::updatepos(){
    speed+=acc;
    pos+=speed;
    speed.limit(maxspeed);
    acc*=0;
}
void dot::draw(int _alpha){
    ofSetColor(255,255,255, _alpha);
    ofDrawLine(prepos.x, prepos.y, pos.x, pos.y);
    prepos = pos;
}
void dot::applyforce(ofVec2f force){
    acc = force/m;
}

void dot::edge(){
    if(pos.x>ofGetWidth()){
        pos.x = 0;
        prepos.x = pos.x;
    }
    if(pos.x<0){
        pos.x = ofGetWidth();
        prepos.x = pos.x;
    }
    if(pos.y>ofGetHeight()){
        pos.y = 0;
        prepos.y = pos.y;
    }
    if(pos.y<0){
        pos.y = ofGetHeight();
        prepos.y = pos.y;
    }
}

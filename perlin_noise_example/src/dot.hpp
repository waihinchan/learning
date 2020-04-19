//
//  dot.hpp
//  perlin_noise_example
//
//  Created by Waihin Chan on 2020/4/16.
//

#ifndef dot_hpp
#define dot_hpp
#include "ofMain.h"
#include <stdio.h>

class dot {
    
public:
    
    float maxspeed;
    float m;
    ofVec2f speed;
    ofVec2f acc;
    ofVec2f pos;
    ofVec2f prepos;
    
    dot(ofVec2f ran_init_pos = ofVec2f(ofRandomWidth(), ofRandomHeight()), float ran_max_speed = 2,float _m = 1);

//    dot(ofVec2f ran_init_pos, float ran_max_speed);

    void draw(int _alpha);
    void updatepos();
    void applyforce(ofVec2f _force);
    void edge();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};





#endif /* dot_hpp */

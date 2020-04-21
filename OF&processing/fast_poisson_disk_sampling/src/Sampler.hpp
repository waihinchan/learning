//
//  Sampler.hpp
//  fast_poisson_disk_sampling
//
//  Created by Waihin Chan on 2020/4/20.
//

#ifndef Sampler_hpp
#define Sampler_hpp

#include <stdio.h>
#include "ofApp.h"

class Sampler{

private:
    int r;
    int k;
    int dimension;
    int cols;
    int rows;
    int w;
    bool caculate_distance(ofVec2f _next_candicate_sample);
    
    ofVec2f search_candicate(ofVec2f _sampling_pos);
    
public:
    std::unique_ptr<ofVec2f[]> grid;
    std::vector<ofVec2f> active;
    std::vector<ofVec2f> samples;
    void init(int _r = 5, int _k = 30, int _dimension= 2);
    void sampling();
    void visualization();

};









#endif /* Sampler_hpp */

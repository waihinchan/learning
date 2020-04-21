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
    //这个是点和点之间的距离
    int k;
    //这个是尝试的次数
    int dimension;
    //这个是多少维 一般就只有3维度 但其实我觉得应该要做一个限制
    int cols;
    //这个是根据w来分割栅格
    int rows;
    //同上
    int w;
    //这个是根据r来划分
    bool caculate_distance(ofVec2f _next_candicate_sample);
    
    ofVec2f search_candicate(ofVec2f _sampling_pos);
    
public:
    std::unique_ptr<ofVec2f[]> grid;
    //本来打算全用array的，但发现active其实是动态的 用array搞得太复杂没有意义。
    std::vector<ofVec2f> active;//这个用来储存临时的采样器
    std::vector<ofVec2f> samples;
    void init(int _r = 10, int _k = 30, int _dimension= 2);
    void sampling();
    void visualization();

};









#endif /* Sampler_hpp */

//
//  Sampler.cpp
//  fast_poisson_disk_sampling
//
//  Created by Waihin Chan on 2020/4/20.
//
//确定需要的点数n，创建一个网格，确保网格格子数大于等于n，让每个格子里有至多一个点
//随机选择一个点，创建两个数组，输出数组，以及处理数组，将这个点放进这两个数组
//随机选择处理数组中的一个点，将这个点从处理数组中去掉
//在点的周围的一共环形里生成一系列随机的新点，30个左右
//删掉与原有的点相隔的太近的点
//将剩下的点加入输出数组，处理数组
//如果处理数组中还有点，继续第3步，否则输出数组就是结果。

#include "Sampler.hpp"

void Sampler::init(int _r, int _k, int _dimension){
    r = _r;
    k = _k;
    dimension = _dimension;
    w =  floor(r/sqrt(dimension));
    //2*(w/2)^2 = r^2
    cols = floor(ofGetHeight()/w);
    rows = floor(ofGetWidth()/w);
    std::cout<<"w= "<< w <<endl;
    std::cout<<"cols= "<< cols <<endl;
    std::cout<<"rows= "<< rows <<endl;
    grid = std::unique_ptr<ofVec2f[]>(new ofVec2f[cols*rows]{ofVec2f(-1, -1)});
    std::cout<<"succeed init! "<<"grid_amount "<< cols*rows <<endl;
    ofVec2f init_random_pos =  ofVec2f(ofRandomWidth(), ofRandomHeight());
    active.push_back(init_random_pos);
    int i = floor(init_random_pos.x/w);
    int j = floor(init_random_pos.y/w);
    grid[j*rows+i] = init_random_pos;
    


    std::cout<<"init point "<<init_random_pos<<" index in grid "<<i*rows+j<<endl;

}

void Sampler::sampling(){
    while(!active.empty()){
            int random_active_index = floor(ofRandom(active.size()));

            ofVec2f sampling_pos = active[random_active_index];
            std::cout<<random_active_index<<" in size "<<active.size()<<endl;
            bool found = false;
            for(int search = 0; search<k;search++){
                ofVec2f next_candicate_sample = search_candicate(sampling_pos)+sampling_pos;
                
                if(caculate_distance(next_candicate_sample)){
                    std::cout<<" push "<<next_candicate_sample<<endl;
                    active.push_back(next_candicate_sample);
                    samples.push_back(next_candicate_sample);
                    int x = floor(next_candicate_sample.x/w);
                    int y = floor(next_candicate_sample.y/w);
                    grid[y*rows+x] = next_candicate_sample;
                    found = true;
                }
                
            }
            
            if(!found){
                active.erase(active.begin()+random_active_index-1);
                std::cout<<"delete"<<random_active_index<<endl;
            }
    }
}




ofVec2f Sampler::search_candicate(ofVec2f _sampling_pos){
    float random_R = ofRandom(r, 2*r);
    float random_direction = ofRandom(0,TWO_PI);
    return ofVec2f(cos(random_direction)*random_R,sin(random_direction)*random_R);

}
bool Sampler::caculate_distance(ofVec2f _next_candicate_sample){
    
    int x = floor(_next_candicate_sample.x/w);
    int y = floor(_next_candicate_sample.y/w);

    bool match = false;
    if(x>=1&&y>=1&&x<=rows-1&&y<=cols-1){
        if(grid[y*rows+x].x!=-1&&grid[y*rows+x].y!=-1){
            match = true;
            for(int i = -1;i<1;i++){
                for(int j = -1;j<1;j++){
                    int index = (j+y)*rows+i+x;
                    if(grid[index].x!=-1&&grid[index].y!=-1){
                        if(grid[index].distance(_next_candicate_sample)<r){
                            match = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    return match;
}

void Sampler::visualization(){
    ofSetColor(255);
    for(int i = 0 ; i < samples.size();i++){
        ofDrawCircle(samples[i].x,samples[i].y,1);
    }
}

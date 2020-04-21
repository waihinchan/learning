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
//
//————————————————
//版权声明：本文为CSDN博主「哗啦cl」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/qq_43439240/article/details/104328586
#include "Sampler.hpp"
//#include "Math.h"
void Sampler::init(int _r, int _k, int _dimension){
    
    r = _r;
    k = _k;
    dimension = _dimension;
    w =  r/sqrt(dimension);
    //2*(w/2)^2 = r^2
    cols = floor(ofGetHeight()/w);
    rows = floor(ofGetWidth()/w);
    grid = std::unique_ptr<ofVec2f[]>(new ofVec2f[cols*rows]{ofVec2f(-1, -1)});
    
    //用指针的缺点就是如果我要数组长度还是很麻烦 如果你不知道的话你的逻辑的话很难搞出来 这个有点像array练习（说白了C++做这行还是不太适合的感觉）
    //step1 随便初始化一个点
    ofVec2f init_random_pos =  ofVec2f(ofRandomWidth(), ofRandomHeight());
    //这里还有一个问题是 如果我是采样像素的话 我要把这个改成floor 或者怎么样让他能够一一对应像素呢 感觉这里算法要再去细化
    active.push_back(init_random_pos);
    int i = floor(init_random_pos.x/w);
    int j = floor(init_random_pos.y/w);
    grid[i*rows+j] = init_random_pos;
    //    j*width+i
    std::cout<<"succeed init! "<<"grid_amount "<< cols*rows <<endl;
    std::cout<<"init point"<<init_random_pos<<"index in grid "<<i*rows+j<<endl;

}

void Sampler::sampling(){
    if(!active.empty()){
        for(int ii = 0; ii<25;ii++){//这个决定一次画多少个点 可要可不要 记得上下中括号扩掉
    //        step1:生成一个随机点
            if(!active.empty()){
            int random_active_index = floor(ofRandom(active.size()));
            std::cout<<random_active_index<<"size"<<active.size()<<endl;
            ofVec2f sampling_pos = active[random_active_index];
                //这句有点问题 看看是不是随机的问题
    //        step2:以这个点为圆心 寻找k次 外环（r到2r的点）
            bool found = false;
            for(int search = 0; search<k;search++){
                ofVec2f next_candicate_sample = search_candicate(sampling_pos)+sampling_pos;
                //这里我们一共获取k次 以samlingpos为中心 距离为r到2r的随机点
                //下一步计算距离 把这个点丢进去就可以了。 直接返回true或者false
                
                //如果找到match 就把这个点放到grid和active
                if(caculate_distance(next_candicate_sample)){
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
        //把这个删除掉。
            }
        }
    }
    }



ofVec2f Sampler::search_candicate(ofVec2f _sampling_pos){
    float random_R = ofRandom(r, 2*r);
    float random_direction = ofRandom(TWO_PI);
    return ofVec2f(cos(random_direction)*random_R,sin(random_direction)*random_R);
}
bool Sampler::caculate_distance(ofVec2f _next_candicate_sample){
    
//    if(_next_candicate_sample)
    int x = floor(_next_candicate_sample.x/w);
    int y = floor(_next_candicate_sample.y/w);
    //这里先把候选点映射到w范围内
    bool match = false;
    if(x>=-1&&y>=-1&&x<=rows-1&&y<=cols-1){//这里检查是否越界 即不能出框 step1
        if(grid[y*rows+x].x!=-1&&grid[y*rows+x].y!=-1){
            //这里检查他自己是否为空
            match = true;
            //match要写在这里 因为要match首先要符合你占据的grid是没有东西且没有越界的
            for(int i = -1;i<1;i++){
                for(int j = -1;j<1;j++){
                    //这个循环嵌套检查周围的8个点
                    
                    int index = (j+y)*rows+i+x;
                    
                    if(grid[index].x!=-1&&grid[index].y!=-1){
                           //这里检查他的邻居是否为空 如果不为空 即可以检查边界
                        if(grid[index].distance(_next_candicate_sample)<r){
                            match = false;
                            //只要有一个就不符合了
                            break;
                        }
                    }
                }
            }
        }
    }
    
    //基本上这里的循环就是如果一开始的点周围都是没有邻居的 所以基本上虽然到检查邻居是否为空那一步就结束了
    return match;
}

void Sampler::visualization(){
    ofSetColor(255);
//    ofSetLineWidth(1);
    for(int i = 0 ; i < samples.size();i++){
        ofDrawCircle(samples[i].x,samples[i].y,1);
    }
}

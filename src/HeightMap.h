//
//  HeightMap.h
//  RenderingWorld
//
//  Created by Jean Caillé on 12/03/13.
//
//

#ifndef __RenderingWorld__HeightMap__
#define __RenderingWorld__HeightMap__

#include <iostream>
#include "cinder/Channel.h"
#include "cinder/Perlin.h"
#include <vector>

using namespace std;

class PerlinHeightMap{
    public :
   
    PerlinHeightMap() ;
    void setup();
    float getHeight(float x, float y);
    float getNormalizedHeight(float x, float y);
    
    //Variables
    int depth ;
    int channelSize ;
    float textureScale ;
    
    float minimumHeight ;
    float maximumHeight ;
    
    vector<cinder::Perlin> harmonic ;
    vector<float> coefficient ;
    float coefficientSum ;
    cinder::Channel8u heightMapChannel ;
};

#endif /* defined(__RenderingWorld__HeightMap__) */

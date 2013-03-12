//
//  HeightMap.cpp
//  RenderingWorld
//
//  Created by Jean Caillé on 12/03/13.
//
//

#include "HeightMap.h"
#include <iostream>
#include "cinder/Channel.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

using namespace std;
using namespace ci ;

PerlinHeightMap::PerlinHeightMap(){
}

void PerlinHeightMap::setup(){
    depth = 15 ;
    channelSize = 512 ;
    cout << "Adding harmonics" << endl ;
    for(int i = 1 ; i <= depth ; i++){
        harmonic.push_back(Perlin((i*i), Rand::randInt()));
        float currentCoeff = (depth *  i) * (depth *  i);
        coefficient.push_back(currentCoeff);
        coefficientSum += currentCoeff;
    }
    
    cout << "Creating image" << endl ;
    
    heightMapChannel = Channel8u(channelSize, channelSize);
    Channel::Iter iter = heightMapChannel.getIter( heightMapChannel.getBounds());
    while(iter.line()){
        while(iter.pixel()){
            Vec2f current = iter.getPos();
            iter.v() = 255.0f * getNormalizedHeight(current.x / channelSize , current.y / channelSize);
//            cout << getNormalizedHeight(current.x / channelSize , current.y / channelSize) << endl ;
        }
    }
    
    cout << "Done" << endl ;
}

float PerlinHeightMap::getHeight(float x, float y){
    float res = 0.0 ;
    for(int i = 0 ; i < depth ; i ++){
        res += coefficient.at(i) * harmonic.at(i).fBm(x,y);
    }
    return res ;
}

float PerlinHeightMap::getNormalizedHeight(float x, float y){
    float height = getHeight(x, y);
    return ((height / coefficientSum) + 1.0f) / 2.0f ;
}

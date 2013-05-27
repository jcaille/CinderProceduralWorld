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
    depth = 20 ;
    channelSize = 100 ;
    Rand::randomize();
    cout << "Adding harmonics" << endl ;
    for(int i = 1 ; i <= depth ; i++){
        harmonic.push_back(Perlin(i+3, Rand::randInt()));
        float currentCoeff = (20 -  i) * (20 -  i);
        coefficient.push_back(currentCoeff);
        coefficientSum += currentCoeff;
    }
    
    
    

    cout << "Creating image" << endl ;
    
    heightMapChannel = Channel8u(channelSize, channelSize);
    
    Channel::Iter iter = heightMapChannel.getIter( heightMapChannel.getBounds());
    maximumHeight = -100000000.0 ;
    minimumHeight = 1000000001.0 ;
    while(iter.line()){
        while(iter.pixel()){
            Vec2f current = iter.getPos();
            float real_value = getHeight(current.x / channelSize , current.y / channelSize);
            if( real_value > maximumHeight){
                maximumHeight = real_value ;
            }
            if( real_value < minimumHeight){
                minimumHeight = real_value;
            }
        }
    }
    
    iter = heightMapChannel.getIter( heightMapChannel.getBounds());
    while(iter.line()){
        while(iter.pixel()){
            Vec2f current = iter.getPos();
            float value = 255.0f * ((getHeight(current.x / channelSize , current.y / channelSize) - minimumHeight) / (maximumHeight - minimumHeight));
            
//            DEBUG
            
//            cout << " ------ " << endl ;
//            cout << getHeight(current.x / channelSize, current.y / channelSize) << endl;
//            cout << ((getHeight(current.x / channelSize , current.y / channelSize) - minimumHeight) / (maximumHeight - minimumHeight)) << endl;
//            cout << value << endl ;
            
            iter.v() = value ;
        }
    }
    
    cout << "Done" << endl ;
    cout << "Sum of coefficients : " << coefficientSum << endl ;
    cout << "Height rangig from " << minimumHeight << " to " << maximumHeight << endl ;
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
    return (height - minimumHeight) / (maximumHeight - minimumHeight);
}

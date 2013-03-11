//
//  WorldMesh.h
//  RenderingWorld
//
//  Created by Jean Caillé on 11/03/13.
//
//

#ifndef __RenderingWorld__WorldMesh__
#define __RenderingWorld__WorldMesh__

#include <iostream>
#include "cinder/Perlin.h"
#include "cinder/TriMesh.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"

class WorldMesh{
    public :
//    METHODS
    WorldMesh();
    
    void setup();
    void update();
    void draw();
    void loadShader();
    cinder::Vec3f pointAt(float x, float y);
    
//    VARIABLES
    int size ;
    int resolution ;
    float step ;
    
    cinder::Perlin roughHeightMap;
    cinder::Perlin fineHeightMap ;
    cinder::Perlin textureHeightMap ;
    
    float roughHeightMultiplicator ;
    float fineHeightMultiplicator ;
    
    float limitWaterGrass ;
    float limitGrassRock ;
    
    cinder::TriMesh mesh ;
    cinder::gl::Texture texture ;
    
    cinder::gl::GlslProg vertexShader;
};

#endif /* defined(__RenderingWorld__WorldMesh__) */

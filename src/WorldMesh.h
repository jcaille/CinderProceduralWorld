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
#include "HeightMap.h"
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
    
//    VARIABLES
    int size ;
    int resolution ;
    float step ;
    PerlinHeightMap heightMap ;
    
    cinder::TriMesh mesh ;
    cinder::gl::Texture grassTexture ;
    cinder::gl::Texture rockTexture ;
    cinder::gl::Texture heightTexture ;
    
    cinder::gl::GlslProg vertexShader;
};

#endif /* defined(__RenderingWorld__WorldMesh__) */

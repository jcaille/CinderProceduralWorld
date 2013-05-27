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
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Vector.h"
#include "Water.h"

class WorldMesh{
    public :
//    METHODS
    WorldMesh();
    
    void setup();
    void update(cinder::Vec3f cameraPosition, bool fog);
    void draw();
    void loadShader();
    
//    VARIABLES
    float size ;
    int resolution ;
    float step ;
    float heightMultiplicator ;
    bool enableFog ;
    
    PerlinHeightMap heightMap ;
    cinder::Vec3f sunPosition ;
    cinder::Vec3f cameraPosition ;
    cinder::TriMesh mesh ;
    
    cinder::gl::Texture grassTexture ;
    cinder::gl::Texture rockTexture ;
    cinder::gl::Texture snowTexture ;
    cinder::gl::Texture dirtTexture ;
    cinder::gl::Texture sandTexture ;
    cinder::gl::Texture heightTexture ;
    
    cinder::gl::GlslProg vertexShader;
    
    Water water;
    
    //PARAMETERS
    cinder::params::InterfaceGl parameters;
    
    float dirtLimit ;
    float sandGrassLimit ;
    float sandGrassBuffer ;
    float sandGrassSlopeModifier ;
};

#endif /* defined(__RenderingWorld__WorldMesh__) */

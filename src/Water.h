//
//  Water.h
//  RenderingWorld
//
//  Created by Jean Caillé on 15/03/13.
//
//

#ifndef __RenderingWorld__Water__
#define __RenderingWorld__Water__

#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Vector.h"
#include "cinder/TriMesh.h"
#include "cinder/params/Params.h"

class Water{
    public :
    Water();
    void setup(float h, float s);
    void update(cinder::Vec3f sunPosition, cinder::Vec3f cameraPosition, bool fog);
    void draw();
    void loadShader();
    void createHeightMap();
    
    cinder::gl::Texture heightMap ;
    cinder::gl::Texture waterTexture ;
    cinder::gl::GlslProg lightningShader ;
    
    float height ;
    float size ;
    float specularityExponent ;
    float yNormalValue ;
    bool enableFog ;
    cinder::Vec3f sunPosition;
    cinder::Vec3f cameraPosition;
    
    cinder::TriMesh waterMesh ;
    };




#endif /* defined(__RenderingWorld__Water__) */

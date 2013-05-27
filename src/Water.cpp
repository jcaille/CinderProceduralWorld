//
//  Water.cpp
//  RenderingWorld
//
//  Created by Jean Caillé on 15/03/13.
//
//

#include "Water.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Vector.h"
#include "cinder/ImageIo.h"
#include "cinder/app/AppBasic.h"
#include "cinder/TriMesh.h"
#include "cinder/Perlin.h"
#include "cinder/Channel.h"
#include "cinder/params/Params.h"

using namespace std;
using namespace ci;
using namespace ci::app;

Water::Water(){
    
}

void Water::loadShader(){
    try {
        int32_t maxGeomOutputVertices;
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
        
        cout << maxGeomOutputVertices <<endl ;
        string glslVersion = string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
        uint32_t spaceIndex = glslVersion.find_first_of(" ");
        if (spaceIndex > 0)
            glslVersion = glslVersion.substr(0, spaceIndex);
        cout << "GLSL version: " + glslVersion << endl;
        
        lightningShader = gl::GlslProg(loadResource("vertex.glsl"), loadResource("water_frag.glsl"));
    }
    catch (gl::GlslProgCompileExc ex) {
        
		// Exit application if shaders failed to compile
		cout << "Unable to compile shaders. Quitting." <<endl;
		cout << ex.what() <<endl;
        cout << "bug" << endl;
	}
}

void Water::setup(float h, float s){
    height = h ;
    size = s ;
    
    specularityExponent = 30.0 ;
    yNormalValue = 5.5 ;
    
    waterTexture = gl::Texture(loadImage(loadResource("water.jpg")));
    
    waterMesh.appendVertex(Vec3f(-size/2,height, -size/2));
    waterMesh.appendTexCoord(Vec2f(0,0));
    waterMesh.appendVertex(Vec3f(size/2, height, -size/2));
    waterMesh.appendTexCoord(Vec2f(1,0));
    waterMesh.appendVertex(Vec3f(size/2, height, size/2));
    waterMesh.appendTexCoord(Vec2f(1,1));
    waterMesh.appendVertex(Vec3f(-size/2, height, size/2));
    waterMesh.appendTexCoord(Vec2f(0,1));
    
    waterMesh.appendTriangle(0, 1, 2);
    waterMesh.appendTriangle(0, 2, 3);
    
    loadShader();
    createHeightMap();

}

void Water::createHeightMap(){
    Perlin noise = Perlin(40);
    float channelSize = 400 ;
    cout << "Creating water height map - " << endl ;
    
    Channel heightMapChannel = Channel8u(channelSize, channelSize);
    
    Channel::Iter iter = heightMapChannel.getIter( heightMapChannel.getBounds());
    float max_value = -1.0 ;
    float min_value = 10001.0 ;
    
    while(iter.line()){
        while(iter.pixel()){
            Vec2f current = iter.getPos();
            float value = 255.0F * noise.fBm(5 * current.x/channelSize, 5 * current.y/channelSize);
            if( value > max_value){
                max_value = value ;
            }
            if( value < min_value){
                min_value = value ;
            }
        }
    }
    
    iter = heightMapChannel.getIter( heightMapChannel.getBounds());
    while(iter.line()){
        while(iter.pixel()){
            Vec2f current = iter.getPos();
            float value = 255.0f * noise.fBm(5 * current.x/channelSize, 5 * current.y/channelSize);
            value = 255.0f * (value - min_value) / (max_value - min_value);
            iter.v() = value ;
        }
    }
    
    heightMap = gl::Texture(heightMapChannel);
    cout << min_value << " " << max_value << endl ;
    cout << "Done creating water height map" << endl;
}

void Water::update(Vec3f s, Vec3f c, bool fog){
    sunPosition = s;
    cameraPosition = c ;
    enableFog = fog ;
}

void Water::draw(){
    
    lightningShader.bind();
    
    lightningShader.uniform("waterTexture", 0);
    lightningShader.uniform("size", size);
    lightningShader.uniform("cameraPosition", cameraPosition);
    lightningShader.uniform("sunPosition", sunPosition);
    lightningShader.uniform("baseHeight", height);
    lightningShader.uniform("waterHeightMap", 1);
    lightningShader.uniform("elapsedTime", (float) app::getElapsedSeconds());
    lightningShader.uniform("yValue", yNormalValue);
    lightningShader.uniform("exponent", specularityExponent);
    lightningShader.uniform("enableFog", enableFog);
    waterTexture.bind(0);
    heightMap.bind(1);
    
    gl::draw(waterMesh);
    
    waterTexture.unbind();
    heightMap.unbind();
    lightningShader.unbind();
    }
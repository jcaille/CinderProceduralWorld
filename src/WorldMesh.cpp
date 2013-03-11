//
//  WorldMesh.cpp
//  RenderingWorld
//
//  Created by Jean Caillé on 11/03/13.
//
//

#include "WorldMesh.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace ci::app;
using namespace std;


WorldMesh::WorldMesh(){
    size = 100 ;
    resolution = 50 ;
    step = (float) size / (float) resolution ;
    
    roughHeightMap = Perlin(2, 12345) ;
    fineHeightMap = Perlin(8, 14567) ;
    textureHeightMap = Perlin(12, 13564);
    
    roughHeightMultiplicator = size / 2.0f ;
    fineHeightMultiplicator = size / 5.0f ;
    
    limitWaterGrass = -20.0f ;
    limitGrassRock = 20.0f ;
    
    mesh = TriMesh();
}

void WorldMesh::loadShader(){
    int32_t maxGeomOutputVertices;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
    
    cout << maxGeomOutputVertices <<endl ;
    string glslVersion = string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    uint32_t spaceIndex = glslVersion.find_first_of(" ");
    if (spaceIndex > 0)
        glslVersion = glslVersion.substr(0, spaceIndex);
    cout << "GLSL version: " + glslVersion << endl;
    
    vertexShader = gl::GlslProg(loadResource("vertex.glsl"));
}

void WorldMesh::setup(){
    for(float i = 0 ; i < resolution ; i ++){
        for(float j = 0 ; j < resolution ; j++){
            float x = i * step - size / 2.0f;
            float y = j * step - size / 2.0f;
            mesh.appendVertex(pointAt(x, y));
            mesh.appendTexCoord(Vec2f(i*step/size, j*step/size));
        }
    }
    
    for(int i = 0 ; i < resolution - 1 ; i++){
        for(int j=0 ; j < resolution - 1 ; j++){
            mesh.appendTriangle(i + size * j, (i+1) + size * j, (i+1) + size * (j+1));
            mesh.appendTriangle(i + size * j, i + size * (j+1), (i+1) + size * (j+1));
        }
    }
    
    cout << mesh.getVertices().size() << endl;
    cout << mesh.getNumTriangles() << endl;
    
//    Compute texture ;
    Surface8u surface = loadImage(loadResource("grass.jpg"));
    texture = gl::Texture(surface);
    
//    SHADER


}

void WorldMesh::update(){
    
}

void WorldMesh::draw(){
    gl::color(1.0f, 1.0f, 1.0f);
    texture.enableAndBind();
    gl::draw(mesh);
    texture.unbind();
}

Vec3f WorldMesh::pointAt(float x, float y){
    float normalized_x = 2 * x / size ;
    float normalized_y = 2 * y / size ;
    float height = roughHeightMap.fBm(normalized_x,normalized_y) * roughHeightMultiplicator + fineHeightMap.fBm(normalized_x, normalized_y) * fineHeightMultiplicator ;
    return Vec3f(x, height, y);
}

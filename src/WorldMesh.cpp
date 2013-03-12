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
#include "HeightMap.h"

using namespace ci;
using namespace ci::app;
using namespace std;


WorldMesh::WorldMesh(){
    size = 1000 ;
    resolution = 200 ;
    step = 1.0f / (float) resolution ;
        
    mesh = TriMesh();
}

void WorldMesh::loadShader(){
    try {
        int32_t maxGeomOutputVertices;
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, & maxGeomOutputVertices);
        
        cout << maxGeomOutputVertices <<endl ;
        string glslVersion = string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
        uint32_t spaceIndex = glslVersion.find_first_of(" ");
        if (spaceIndex > 0)
            glslVersion = glslVersion.substr(0, spaceIndex);
        cout << "GLSL version: " + glslVersion << endl;
        
        vertexShader = gl::GlslProg(loadResource("vertex.glsl"), loadResource("frag.glsl"));
    }
    catch (gl::GlslProgCompileExc ex) {
        
		// Exit application if shaders failed to compile
		cout << "Unable to compile shaders. Quitting." <<endl;
		cout << ex.what() <<endl;
        cout << "bug" << endl;
	}
}

void WorldMesh::setup(){
    //    SHADER
    loadShader();

    heightMap.setup();
    
    for(float i = 0 ; i < resolution ; i ++){
        for(float j = 0 ; j < resolution ; j++){
            float x = i * step;
            float y = j * step;
            Vec3f point( (x-0.5f) * size, heightMap.getHeight(x,y), (y-0.5f)*size);
            mesh.appendVertex(point);
            mesh.appendTexCoord(Vec2f(x,y));
        }
    }
    
    for(int i = 0 ; i < resolution - 1 ; i++){
        for(int j=0 ; j < resolution - 1 ; j++){
            mesh.appendTriangle(i + resolution * j, (i+1) + resolution * j, (i+1) + resolution * (j+1));
            mesh.appendTriangle(i + resolution * j, i + resolution * (j+1), (i+1) + resolution * (j+1));
        }
    }
    
    cout << mesh.getVertices().size() << endl;
    cout << mesh.getNumTriangles() << endl;
    
    //    Compute texture ;
    grassTexture = gl::Texture(loadImage(loadResource("grass.jpg")));
    rockTexture = gl::Texture(loadImage(loadResource("rock.JPG")));
    heightTexture = gl::Texture(heightMap.heightMapChannel);
    
    
}

void WorldMesh::update(){
    
}

void WorldMesh::draw(){
    gl::color(1.0f, 1.0f, 1.0f);
    vertexShader.bind();
    vertexShader.uniform("grassTexture", 0);
    vertexShader.uniform("heightMap", 1);
    vertexShader.uniform("rockTexture", 2);
    
    grassTexture.bind(0);
    heightTexture.bind(1);
    rockTexture.bind(2);
    gl::draw(mesh);
    vertexShader.unbind();
//    gl::draw(heightTexture, app::getWindowBounds());
}



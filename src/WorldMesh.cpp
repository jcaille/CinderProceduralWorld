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
#include "Water.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;


WorldMesh::WorldMesh(){
    size = 10000 ;
    float multiplicator = 40.0f / 1000.0f ;
    resolution = size * multiplicator ;
    step = 1.0f / (float) resolution ;
    sunPosition = Vec3f(30.0f, 500.0f, 0.0f);
    mesh = TriMesh();
    heightMultiplicator =  1500 * size / 5000 ;
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
    
    //  PARAMETERS
    
    
    parameters = params::InterfaceGl( "World", Vec2i( 225, 200 ) );
    
    dirtLimit = 0.65 ;
    parameters.addParam("DirtLimit", &dirtLimit, "min=0.3 max=1.0 step=0.025");
    
    sandGrassLimit = 0.35 ;
    parameters.addParam("Limit sand/grass", &sandGrassLimit, "min=0.25 max=0.40 step=0.025");
    
    sandGrassBuffer = 0.05 ;
    parameters.addParam("Sand/grass buffer", &sandGrassBuffer, "min=0.0 max=0.20 step=0.01");
    
    sandGrassSlopeModifier = 0 ;
    parameters.addParam("Sand/grass slope", &sandGrassSlopeModifier, "min=-0.1 max=0.1 step=0.005");
    
    for(float i = 0 ; i < resolution ; i ++){
        for(float j = 0 ; j < resolution ; j++){
            float x = i * step;
            float y = j * step;
            float realX = (x-0.5f) * size ;
            float realY = (y-0.5f) * size ;
            float realZ = (heightMap.getNormalizedHeight(x, y) - 0.5f) * heightMultiplicator ;
            Vec3f point( realX, realZ, realY);
//            cout << heightMap.getNormalizedHeight(x, y) << endl ;
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
    rockTexture = gl::Texture(loadImage(loadResource("rock.jpg")));
    snowTexture = gl::Texture(loadImage(loadResource("snow.jpg")));
    dirtTexture = gl::Texture(loadImage(loadResource("dirt.jpg")));
    sandTexture = gl::Texture(loadImage(loadResource("sand.jpg")));
    
    heightTexture = gl::Texture(heightMap.heightMapChannel);
    
    //  SETUP WATER
    water.setup(-0.2 * heightMultiplicator, size);
    
    cout << heightMap.getNormalizedHeight(0.5, 0.5) << endl ;
    cout << heightMap.minimumHeight << endl ;
    cout << heightMap.maximumHeight << endl ;
    cout << heightMap.getHeight(0.5, 0.5) << endl ;
}

void WorldMesh::update(Vec3f c, bool fog){
    sunPosition.x = size * 1.5 * cos(app::getElapsedSeconds() / 3.0f);
    sunPosition.z = size * 1.5 * sin(app::getElapsedSeconds() / 3.0f);
    sunPosition.y = heightMultiplicator * 3 + heightMultiplicator * sin(app::getElapsedSeconds() / 40.0f) ;
    water.update(sunPosition, c, fog);
    cameraPosition = c ;
    enableFog = fog ;
}

void WorldMesh::draw(){
    gl::color(1.0f, 1.0f, 1.0f);
        
    rockTexture.setWrap(GL_REPEAT, GL_REPEAT);
    grassTexture.setWrap(GL_REPEAT, GL_REPEAT);
    

    
    vertexShader.bind();
    vertexShader.uniform("heightMap", 0);
    vertexShader.uniform("grassTexture", 1);
    vertexShader.uniform("rockTexture", 2);
    vertexShader.uniform("snowTexture", 3);
    vertexShader.uniform("dirtTexture", 4);
    vertexShader.uniform("sandTexture", 5);
    vertexShader.uniform("sunPosition", sunPosition);
    vertexShader.uniform("cameraPosition", cameraPosition);
    vertexShader.uniform("size", size);
    vertexShader.uniform("heightMultiplicator", heightMultiplicator);
    vertexShader.uniform("enableFog", enableFog);
    
    //PARAMETERS
    vertexShader.uniform("dirtLimit", dirtLimit);
    vertexShader.uniform("sandGrassLimit", sandGrassLimit);
    vertexShader.uniform("sandGrassBuffer", sandGrassBuffer);
    vertexShader.uniform("sandGrassSlopeModifier", sandGrassSlopeModifier);
    //TEXTURES
    dirtTexture.bind(4);
    snowTexture.bind(3);
    rockTexture.bind(2);
    grassTexture.bind(1);
    heightTexture.bind(0);
    sandTexture.bind(5);
    
    gl::draw(mesh);
    
    vertexShader.unbind();
    
    dirtTexture.unbind();
    snowTexture.unbind();
    rockTexture.unbind();
    grassTexture.unbind();
    heightTexture.unbind();
    sandTexture.unbind();
    
    parameters.draw();
    water.draw();
    
//    gl::draw(heightTexture, app::getWindowBounds());
}



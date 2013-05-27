#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "WorldMesh.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RenderingWorldApp : public AppBasic {
  public:
    void prepareSettings( Settings *settigs);
	void setup();
	void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
    void keyUp( KeyEvent event);
	void update();
	void draw();
    Vec3f computeCenter(float e, float d);
    
    CameraPersp cam ;
    Vec3f eye, center, up ;
    float elevation, direction ;
    float lastFrameTime ;
    
    params::InterfaceGl parameters;
    Quatf sceneRotation ;
    
    WorldMesh world ;
    gl::Texture skyTexture ;
    gl::Texture sunTexture ;
    bool movingForward, movingBackward, turningLeft, turningRight, lookingUp, lookingDown, enableFog, canFly ;
};


Vec3f RenderingWorldApp::computeCenter(float e, float d){
    float pi = atan(1)*4 ;
    float x = sin(pi/2.0f - e) * cos(d);
    float z = sin(pi/2.0f - e) * sin(d);
    float y = cos(pi/2.0f - e) ;
    return Vec3f(x,y,z);
}

void RenderingWorldApp::prepareSettings( Settings *settings ){
    settings->setWindowSize( 1000 , 800 );
    settings->setFrameRate( 30.0f );
}

void RenderingWorldApp::setup()
{
//    parameters = params::InterfaceGl( "World", Vec2i( 225, 200 ) );
//    parameters.addParam("Rotation", &sceneRotation);
    
    world = WorldMesh() ;
    world.setup();
    
    cam = CameraPersp();
    cam.setPerspective( 60.0f, getWindowAspectRatio(), 5.0f, 2000.0f );
    
    elevation = 0.0f ;
    direction = 0.0f ;
    
    eye = Vec3f(0,0,1);
    center = eye + computeCenter(elevation, direction) ;
    up = Vec3f::yAxis() ;
    
    movingForward = false ;
    movingBackward = false ;
    turningLeft = false ;
    turningRight = false ;
    lookingDown = false ;
    lookingUp = false ;
    enableFog = false ;
    canFly = true ;
    
    gl::enableDepthWrite();
	gl::enableDepthRead();
    gl::enableAlphaBlending(true);
    skyTexture = gl::Texture(loadImage(loadResource("sky.png")));
    sunTexture = gl::Texture(loadImage(loadResource("sun.png")));

}

void RenderingWorldApp::mouseDown( MouseEvent event )
{
}

void RenderingWorldApp::keyDown(cinder::app::KeyEvent event){
    if(event.getChar() == 'z'){
        movingForward = true ;
    }
    if(event.getChar() == 's'){
        movingBackward = true ;
    }
    if(event.getChar() == 'd'){
        turningRight = true ;
    }
    if(event.getChar() == 'q'){
        turningLeft = true ;
    }
    if(event.getChar() == 'a'){
        lookingUp = true ;
    }
    if(event.getChar() == 'e'){
        lookingDown = true ;
    }
    if(event.getChar() == 'f'){
        enableFog = !enableFog ;
    }
    if(event.getChar() == 'r'){
        canFly = !canFly ;
    }
}

void RenderingWorldApp::keyUp(cinder::app::KeyEvent event){
    if(event.getChar() == 'z'){
        movingForward = false ;
    }
    if(event.getChar() == 's'){
        movingBackward = false ;
    }
    if(event.getChar() == 'd'){
        turningRight = false ;
    }
    if(event.getChar() == 'q'){
        turningLeft = false ;
    }
    if(event.getChar() == 'a'){
        lookingUp = false ;
    }
    if(event.getChar() == 'e'){
        lookingDown = false ;
    }

}

void RenderingWorldApp::update()
{
    float pi = atan(1) * 4 ;
    float currentTime = app::getElapsedSeconds();
    float elapsedTime = (currentTime - lastFrameTime) * 60.0f ;
    
    if(turningLeft){
        direction -= elapsedTime * 0.02f ;
    }
    if(turningRight){
        direction += elapsedTime *0.02f ;
    }
    if(lookingUp){
        elevation = min(pi/2 - 0.01f, elevation + elapsedTime * 0.02f);
    }
    if(lookingDown){
        elevation = max(-pi/2 + 0.01f, elevation - elapsedTime * 0.02f);
    }

    if(canFly){
        if(movingForward){
            Vec3f movementDirection = center - eye ;
            movementDirection.normalize();
            eye += elapsedTime * movementDirection * 4 ;
        }
        if(movingBackward){
            Vec3f movementDirection = center - eye ;
            movementDirection.normalize();
            eye -= elapsedTime * movementDirection * 4;
        }
        center = eye + computeCenter(elevation, direction);
        cam.lookAt( eye, center, up );
    } else {
        if(movingForward){
            Vec3f movementDirection = center - eye ;
            movementDirection.y = 0 ;
            movementDirection.normalize();
            eye += elapsedTime * movementDirection * 2 ;
            eye.y = 15.0f + world.heightMultiplicator * (world.heightMap.getNormalizedHeight(eye.x / world.size +0.5f, eye.z / world.size +0.5f) - 0.5) ;
        }
        if(movingBackward){
            Vec3f movementDirection = center - eye ;
            movementDirection.y = 0 ;
            movementDirection.normalize();
            eye -= elapsedTime * movementDirection * 2;
            eye.y = 15.0f + world.heightMultiplicator * (world.heightMap.getNormalizedHeight(eye.x / world.size + 0.5f, eye.z / world.size +0.5f) - 0.5) ;
        }
        center = eye + computeCenter(elevation, direction);
        cam.lookAt( eye, center, up );
    }
    gl::setMatrices( cam );
    gl::rotate( sceneRotation) ;
    world.update(center, enableFog);
    lastFrameTime = currentTime;
}

void RenderingWorldApp::draw()
{
    gl::clear(Color(1.0f, 1.0f, 1.0f));

    gl::enableDepthWrite();
	gl::enableDepthRead();
    gl::enableAlphaBlending();
    
    skyTexture.enableAndBind();
    gl::drawSphere(eye, 2000);
    skyTexture.unbind();
    
	world.draw();
    
    gl::color(1.0f, 1.0f, 1.0f);
    
    Vec3f sunDirection = (world.sunPosition - eye);
    sunDirection.normalize();
    Vec3f normalizedSunPosition = eye + 1500.0f * sunDirection ;
    sunTexture.enableAndBind();
    gl::drawBillboard(normalizedSunPosition, Vec2f(300,300), 0, Vec3f::yAxis(), Vec3f::yAxis().cross(sunDirection));
    sunTexture.unbind();
    
    gl::color(1.0f, 1.0f, 1.0f);


    
}

CINDER_APP_BASIC( RenderingWorldApp, RendererGl )

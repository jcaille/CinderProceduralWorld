#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Material.h"
#include "cinder/Perlin.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "WorldMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class RenderingWorldApp : public AppBasic {
  public:
    void prepareSettings( Settings *settigs);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    CameraPersp cam ;
    Vec3f eye, center, up ;
    
    params::InterfaceGl parameters;
    Quatf sceneRotation ;
    
    WorldMesh world ;
};

void RenderingWorldApp::prepareSettings( Settings *settings ){
    settings->setWindowSize( 800, 600 );
    settings->setFrameRate( 60.0f );
}

void RenderingWorldApp::setup()
{
    parameters = params::InterfaceGl( "World", Vec2i( 225, 200 ) );
    parameters.addParam("Rotation", &sceneRotation);
    
    world = WorldMesh() ;
    world.setup();
    
    cam = CameraPersp();
    cam.setPerspective( 60.0f, getWindowAspectRatio(), 5.0f, 3000.0f );
    
    eye = Vec3f(100.0f, 30.0f, 100.0f);
    center = Vec3f(0.0f, 0.0f, 0.0f);
    up = Vec3f::yAxis() ;

}

void RenderingWorldApp::mouseDown( MouseEvent event )
{
}

void RenderingWorldApp::update()
{
    cam.lookAt( eye, center, up );
    gl::setMatrices( cam );
    gl::rotate(sceneRotation);
}

void RenderingWorldApp::draw()
{
    gl::clear();
    gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();

	world.draw();
//    gl::drawSphere(Vec3f(0.0f, 0.0f, 0.0f), 5.0f);
    parameters.draw();
}

CINDER_APP_BASIC( RenderingWorldApp, RendererGl )

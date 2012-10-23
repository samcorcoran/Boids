#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/Camera.h"
#include "cinder/params/Params.h"
#include "cinder/Text.h"

#include "AgentController.h"
#include "InterfaceParams.h"


#define RESOLUTION 10
#define NUM_AGENTS_TO_SPAWN 1
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
//#define WINDOW_WIDTH 1200
//#define WINDOW_HEIGHT 900
#define BIN_SIZE 200;

using namespace ci;
using namespace ci::app;
using std::vector;

// Controller for managing agents
AgentController mAgentController;

class Boids : public AppBasic {
 public:
	// 
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );

	// Mouse events
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	bool mIsPressed;

	// Mouse positions
	Vec2f mWindowDim;
	Vec2i mMouseLoc;
	Vec2f mMouseVel;

	// Loop events
	void setup();
	void update();
	void draw();

	// Initialisation and setup
	void setupParameters();
	void setupCamera();
	void setupUIParameters();
	
	// Text layout for HUD
	Font mFont;
	//TextLayout mLayout;
	//Texture mLayoutTexture;

	// Drawing
	void drawLine(const Vec3f &start, const Vec3f &end);
	
	// Camera
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mCameraDistance;

	// Parameters ui panel
	params::InterfaceGl mParams;
	// Parameters storage
	InterfaceParams interfaceParams;	
};

void Boids::prepareSettings( Settings *settings )
{
	settings->setWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
	settings->setFrameRate( 60.0f );
}

void Boids::mouseDown( MouseEvent event )
{
	mIsPressed = true;
	mAgentController.addAgents( NUM_AGENTS_TO_SPAWN, Vec2f(mMouseLoc.x, mMouseLoc.y) );
}

void Boids::mouseUp( MouseEvent event )
{
	console() << "UP mouse loc: " << mMouseLoc << std::endl;
	mIsPressed = false;
}

void Boids::mouseMove( MouseEvent event )
{
	mMouseVel = ( event.getPos() - mMouseLoc );
	mMouseLoc = event.getPos();
}

void Boids::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void Boids::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' ){
		interfaceParams.drawAgents = !interfaceParams.drawAgents;
	}
}

void Boids::setup()
{	
	mMouseLoc = Vec2i( 0, 0 );

	mMouseVel = Vec2f::zero();
	interfaceParams.drawAgents = true;
	mIsPressed = false;
	mWindowDim = Vec2f(app::getWindowWidth(), app::getWindowHeight());

	// Set up agent controller and bins
	const int binSize = BIN_SIZE;
	const int windowWidth = WINDOW_WIDTH;
	const int windowHeight = WINDOW_HEIGHT;
	mAgentController = AgentController(binSize, windowWidth, windowHeight);

	// Setup the preset parameters
	setupParameters();

	// Setup camera
	setupCamera();

	// Setup interface presets
	setupUIParameters();

	// Setup frame per second HUD
	Font mFont = Font("Quicksand Book Regular", 12.0f);
}

void Boids::update()
{	
	// Update Camera
	mEye = Vec3f( WINDOW_WIDTH/2, WINDOW_HEIGHT/2, -mCameraDistance);
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );

	// Update Agents
	mAgentController.update( Vec2f(mMouseLoc.x, mMouseLoc.y), interfaceParams);
}

void Boids::draw()
{	
	gl::clear( Color( 0, 0, 0 ), true );
	
	// Draw borders
	gl::color( Color(0.3f, 0.3f, 0.3f) );
	drawLine(Vec3f(0.0f, 0.0f, 0.0f),Vec3f(0.0f, WINDOW_HEIGHT, 0.0f));
	drawLine(Vec3f(0.0f, WINDOW_HEIGHT, 0.0f), Vec3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f));
	drawLine(Vec3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f), Vec3f(WINDOW_WIDTH, 0.0f, 0.0f));
	drawLine(Vec3f(WINDOW_WIDTH, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f));

	// Draw agents
	if( interfaceParams.drawAgents ){
		glDisable( GL_TEXTURE_2D );
		mAgentController.draw();
	}

	// Draw HUD infos
	float avgFPS = getAverageFps();
	int totalAgentsCount = mAgentController.agentCount();
	gl::drawString( "Framerate: " + toString(avgFPS), Vec2f( 10.0f, 10.0f ), Color::white(), mFont );
	gl::drawString( "Agents: " + toString(totalAgentsCount), Vec2f( 10.0f, 25.0f ), Color::white(), mFont );

	// Draw interface panel
	params::InterfaceGl::draw();
}

void Boids::drawLine( const Vec3f &start, const Vec3f &end )
{
	float lineVerts[3*2];
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, lineVerts );
	lineVerts[0] = start.x; 
	lineVerts[1] = start.y; 
	lineVerts[2] = start.z;
	lineVerts[3] = end.x; 
	lineVerts[4] = end.y; 
	lineVerts[5] = end.z; 
	glDrawArrays( GL_LINES, 0, 2 );
	glDisableClientState( GL_VERTEX_ARRAY );
}

void Boids::setupParameters()
{
	interfaceParams.mSeparationWeight = 20.0f;
	interfaceParams.mAlignmentWeight = 6.0f;
	interfaceParams.mCohesionWeight = 5.0f;

	interfaceParams.mSeparationRange = 40.0f;
	interfaceParams.mAlignmentRange = 50.0f;
	interfaceParams.mCohesionRange = 100.0f;

	
	interfaceParams.mSeparationSamples = 30.0f;
	interfaceParams.mAlignmentSamples = 40.0f;
	interfaceParams.mCohesionSamples = 80.0f;

	interfaceParams.mSeparationCompatabilityThresh = 0.0f;
	interfaceParams.mAlignmentCompatabilityThresh = 0.2f;
	interfaceParams.mCohesionCompatabilityThresh = 0.2f;

	interfaceParams.mRandWeight = 1.0f;
}

void Boids::setupUIParameters()
{
	mParams = params::InterfaceGl( "Flocking", Vec2i( 225, 200 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation );
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1000.0 step=50.0 keyIncr=s keyDecr=w" );

	mParams.addParam( "separationWeight", &interfaceParams.mSeparationWeight, "min=0.0" );
	mParams.addParam( "separationRange", &interfaceParams.mSeparationRange, "min=0.0" );
	mParams.addParam( "separationSamples", &interfaceParams.mSeparationSamples, "min=0.0" );
	mParams.addParam( "separationThresh", &interfaceParams.mSeparationCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "alignmentWeight", &interfaceParams.mAlignmentWeight, "min=0.0" );
	mParams.addParam( "alignmentRange", &interfaceParams.mAlignmentRange, "min=0.0" );
	mParams.addParam( "alignmentSamples", &interfaceParams.mAlignmentSamples, "min=0.0" );
	mParams.addParam( "alignmentThresh", &interfaceParams.mAlignmentCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "cohesionWeight", &interfaceParams.mCohesionWeight, "min=0.0" );
	mParams.addParam( "cohesionRange", &interfaceParams.mCohesionRange, "min=0.0" );
	mParams.addParam( "cohesionSamples", &interfaceParams.mCohesionSamples, "min=0.0" );
	mParams.addParam( "cohesionThresh", &interfaceParams.mCohesionCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "randWeight", &interfaceParams.mRandWeight, "min=0.0" );
}

void Boids::setupCamera()
{
	mCameraDistance = 550.0f;	
	mEye        = Vec3f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, mCameraDistance );
	mCenter     = Vec3f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0.0f);
	mUp         = -Vec3f::yAxis();
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 5.0f, 3000.0f );
}

CINDER_APP_BASIC( Boids, RendererGl )

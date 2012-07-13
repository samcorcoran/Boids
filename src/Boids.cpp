#include "cinder/app/AppBasic.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/Texture.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/Camera.h"
#include "AgentController.h"
#include "cinder/params/Params.h"

#define RESOLUTION 10
#define NUM_AGENTS_TO_SPAWN 1
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define BIN_SIZE 25;

using namespace ci;
using namespace ci::app;

class Boids : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void setup();
	void update();
	void draw();
	void drawLine(const Vec3f &start, const Vec3f &end);
	
	Vec2f mWindowDim;
	Vec2i mMouseLoc;
	Vec2f mMouseVel;
	bool mIsPressed;
	
	// Camera
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	Vec3f				mEye, mCenter, mUp;
	float				mCameraDistance;

	//params
	params::InterfaceGl mParams;
	float mSeparationWeight;
	float mAlignmentWeight;
	float mCohesionWeight;
	float mRandWeight;

	float mSeparationRange;
	float mAlignmentRange;
	float mCohesionRange;
	
	float mSeparationSamples;
	float mAlignmentSamples;
	float mCohesionSamples;

	float mSeparationCompatabilityThresh;
	float mAlignmentCompatabilityThresh;
	float mCohesionCompatabilityThresh;

	AgentController mAgentController;
	
	bool mDrawAgents;
};

void Boids::prepareSettings( Settings *settings )
{
	settings->setWindowSize( WINDOW_WIDTH, WINDOW_HEIGHT );
	settings->setFrameRate( 60.0f );
}

void Boids::setup()
{	
	mMouseLoc = Vec2i( 0, 0 );
	mMouseVel = Vec2f::zero();
	mDrawAgents = true;
	mIsPressed = false;
	mWindowDim = Vec2f(app::getWindowWidth(), app::getWindowHeight());

	mCameraDistance = 550.0f;

	bool test = false;
	//Standard Params
	
	if ( !test)
	{
		mSeparationWeight = 20.0f;
		mAlignmentWeight = 6.0f;
		mCohesionWeight = 5.0f;
		mRandWeight = 1.0f;

		mSeparationRange = 40.0f;
		mAlignmentRange = 50.0f;
		mCohesionRange = 100.0f;
	
		mSeparationSamples = 30.0f;
		mAlignmentSamples = 40.0f;
		mCohesionSamples = 80.0f;

		mSeparationCompatabilityThresh = 0.0f;
		mAlignmentCompatabilityThresh = 0.2f;
		mCohesionCompatabilityThresh = 0.2f;
	}
	else{
		//TestParams
	
		mSeparationWeight = 15.0f;
		mAlignmentWeight = 0.0f;
		mCohesionWeight = 0.0f;
		mRandWeight = 10.0f;

		mSeparationRange = 80.0f;
		mAlignmentRange = 50.0f;
		mCohesionRange = 50.0f;
	
		mSeparationSamples = 30.0f;
		mAlignmentSamples = 40.0f;
		mCohesionSamples = 80.0f;

		mSeparationCompatabilityThresh = 0.0f;
		mAlignmentCompatabilityThresh = 0.0f;
		mCohesionCompatabilityThresh = 0.0f;
	}

	// SETUP CAMERA
	mEye        = Vec3f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, mCameraDistance );
	mCenter     = Vec3f(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0.0f);
	mUp         = -Vec3f::yAxis();
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 5.0f, 3000.0f );

	//params
	mParams = params::InterfaceGl( "Flocking", Vec2i( 225, 200 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation );
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1000.0 step=50.0 keyIncr=s keyDecr=w" );

	mParams.addParam( "separationWeight", &mSeparationWeight, "min=0.0" );
	mParams.addParam( "separationRange", &mSeparationRange, "min=0.0" );
	mParams.addParam( "separationSamples", &mSeparationSamples, "min=0.0" );
	mParams.addParam( "separationThresh", &mSeparationCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "alignmentWeight", &mAlignmentWeight, "min=0.0" );
	mParams.addParam( "alignmentRange", &mAlignmentRange, "min=0.0" );
	mParams.addParam( "alignmentSamples", &mAlignmentSamples, "min=0.0" );
	mParams.addParam( "alignmentThresh", &mAlignmentCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "cohesionWeight", &mCohesionWeight, "min=0.0" );
	mParams.addParam( "cohesionRange", &mCohesionRange, "min=0.0" );
	mParams.addParam( "cohesionSamples", &mCohesionSamples, "min=0.0" );
	mParams.addParam( "cohesionThresh", &mCohesionCompatabilityThresh, "min=0.0 max=1.0" );

	mParams.addParam( "randWeight", &mRandWeight, "min=0.0" );
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
	//mMouseLoc = Vec2i(mMouseLoc[0], WINDOW_HEIGHT-mMouseLoc[1]);
}

void Boids::mouseDrag( MouseEvent event )
{
	mouseMove( event );
}

void Boids::keyDown( KeyEvent event )
{
	if( event.getChar() == '1' ){
		mDrawAgents = ! mDrawAgents;
	}
}

void Boids::update()
{	
	// UPDATE CAMERA
	mEye = Vec3f( WINDOW_WIDTH/2, WINDOW_HEIGHT/2, -mCameraDistance);
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );

	// Params: cohesion, alignment, separation
	Vec4f ruleWeights = Vec4f(mCohesionWeight, mAlignmentWeight, mSeparationWeight, mRandWeight);
	Vec3f ruleRanges = Vec3f(mCohesionRange, mAlignmentRange, mSeparationRange);
	Vec3f ruleSamples = Vec3f(mCohesionSamples, mAlignmentSamples, mSeparationSamples);
	Vec3f ruleCompatabilityThresholds = Vec3f(mCohesionCompatabilityThresh, mAlignmentCompatabilityThresh, mSeparationCompatabilityThresh);

	mAgentController.update( Vec2f(mMouseLoc.x, mMouseLoc.y), ruleWeights, ruleRanges, ruleSamples, ruleCompatabilityThresholds);
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

	if( mDrawAgents ){
		glDisable( GL_TEXTURE_2D );
		mAgentController.draw();
	}

	params::InterfaceGl::draw();
}

void Boids::drawLine( const Vec3f &start, const Vec3f &end )
{
 float lineVerts[3*2];
 glEnableClientState( GL_VERTEX_ARRAY );
 glVertexPointer( 3, GL_FLOAT, 0, lineVerts );
 lineVerts[0] = start.x; lineVerts[1] = start.y; lineVerts[2] = start.z;
 lineVerts[3] = end.x; lineVerts[4] = end.y; lineVerts[5] = end.z; 
 glDrawArrays( GL_LINES, 0, 2 );
 glDisableClientState( GL_VERTEX_ARRAY );
}

CINDER_APP_BASIC( Boids, RendererGl )

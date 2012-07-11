#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "AgentController.h"

using namespace ci;
using namespace ci::app;
using std::list;


AgentController::AgentController()
{
}

void AgentController::update( const Vec2i &mouseLoc, const Vec4f &ruleWeights, const Vec3f &ruleRanges, const Vec3f &ruleSamples, const Vec3f &ruleCompatabilityThresholds)
{
	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); ) {
		if( p->mIsDead ){
			p = mAgents.erase( p );
		} else {
			p->update( mouseLoc, mAgents, ruleWeights, ruleRanges, ruleSamples, ruleCompatabilityThresholds );
			clampLocToTorus(p->mLoc);
			++p;
		}
	}
}

void AgentController::draw()
{
	//console() << "Drawing agents: " << mAgents.size() << std::endl;
	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); ++p ){
		p->draw();
	}
}

void AgentController::addAgents( int amt, const Vec2i &mouseLoc)
{
	for( int i=0; i<amt; i++ )
	{
		Vec2f loc = mouseLoc + Rand::randVec2f() * 5.0f;
		mAgents.push_back( Agent( loc, mAgents.size()+1 ) );
	}
}

void AgentController::removeAgents( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mAgents.pop_back();
	}
}

void AgentController::clampLocToTorus(Vec3f &loc )
{
	Vec2f windowDim = Vec2f(800,600);
	// clamp x
	if (loc.x < 0) { 
		loc.x  += windowDim.x;
	} else if ( loc.x > windowDim.x ) {
		loc.x -= windowDim.x;
	}

	//clamp y
	if (loc.y < 0) { 
		loc.y  += windowDim.y;
	} else if ( loc.y > windowDim.y ) {
		loc.y -= windowDim.y;
	}
}

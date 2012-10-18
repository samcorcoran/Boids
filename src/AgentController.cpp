#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "AgentController.h"
#include "InterfaceParams.h"
#include "Agent.h"
#include "SpatialBin.h"

using namespace ci;
using namespace ci::app;
using std::vector;
using std::list;

AgentController::AgentController()
{
	binSize = 10;
	// Hardcoded bin values
	xBins = 1 + (int) 800 / binSize;
	yBins = 1 + (int) 600 / binSize;
	totalBins = xBins*yBins;

	for(int i = 0; i < totalBins; i++){
		this->gridBins.push_back(SpatialBin());
	}
}

AgentController::AgentController(const int newBinSize)
{
	binSize = newBinSize;
	// Hardcoded bin values
	xBins = 1 + (int) 800 / binSize;
	yBins = 1 + (int) 600 / binSize;
	totalBins = xBins*yBins;

	for(int i = 0; i < totalBins; i++){
		gridBins.push_back(SpatialBin());
	}
}

void AgentController::update( const Vec2i &mouseLoc, InterfaceParams &interfaceParams)
 {
	//for(int i = 0; i < xBins; i++)
	//{
	//	for (int j = 0; j < yBins; j++ )
	//	{
	//		bins[i].clear();
	//	}
	//}

	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); ) {
		if( p->mIsDead ){
			p = mAgents.erase( p );
		} else {

			// Determine this agent's neighbours
			list<Agent*> nearbyAgents;
			getNearbyAgents(nearbyAgents, p->mLoc, p->mVisualDistance);

			// Update agent's location
			p->update( mouseLoc, mAgents, interfaceParams );

			// Ensure location is in bounds
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

void AgentController::sortAgentsIntoBins()
{
	// Binned locations for next round
	vector<list<Agent*>> newBins(totalBins, list<Agent*>(10));

	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); ) {
		if( p->mIsDead ){
			p = mAgents.erase( p );
		}
		else 
		{
			// Update agent's bin for new location
			int binNumber = convertLocToBin(p->mLoc);
			//List &nextList = *newBins[binNumber];
			newBins[binNumber].push_back(p);
		}
	}

	// Replace previous bin locations with new ones
	gridBins = newBins;
}

void AgentController::getNearbyAgents(list<Agent*> &nearbyAgents, Vec3f centreLocation, float radius)
{
	// calculate number of surrounding bins required to search
	int radiusInBins = ceil(radius/binSize);
	int searchGridWidth = (radiusInBins*2)+1;

	// iterate through columns and rows of search grid
	for (int i = -radiusInBins; i < radiusInBins+1; i++)
	{
		for (int j = -radiusInBins;  j < radiusInBins+1; j++)
		{
			int nextBin = convertLocToBin(Vec3f(centreLocation.x + j*binSize, centreLocation.y+i*binSize, 0)); 
			// push pointers for all agents in this bin into nearbyAgents
			//nearbyAgents.push()
		}
	}
}

int AgentController::convertLocToBin(Vec3f &loc)
{
	clampLocToTorus(loc);
	return floor(loc.y*xBins) + ceil(loc.x);
}
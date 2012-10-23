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
	screenWidth = 800;
	screenHeight = 600;
	binSize = 100;

	// Hardcoded bin values
	xBins = (int)( ceil( (float)(screenWidth/binSize)) );
	yBins = (int)( ceil( (float)(screenHeight/binSize)) );
	
	totalBins = xBins*yBins;

	for(int i = 0; i < totalBins; i++){
		gridBins.push_back(SpatialBin());
	}
}

AgentController::AgentController(const int newBinSize, const int newScreenWidth, const int newScreenHeight)
{
	screenWidth = newScreenWidth;
	screenHeight = newScreenHeight;

	binSize = newBinSize;
	// Hardcoded bin values
	xBins = (int)( ceil( (float)(screenWidth/binSize)) );
	yBins = (int)( ceil( (float)(screenHeight/binSize)) );
	
	totalBins = xBins*yBins;

	for(int i = 0; i < totalBins; i++){
		gridBins.push_back(SpatialBin());
	}
}

void AgentController::update( const Vec2i &mouseLoc, InterfaceParams &interfaceParams)
 {
	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); ) {
		if( p->mIsDead ){
			p = mAgents.erase( p );
		} else {

			// Determine this agent's neighbours
			list<Agent*> nearbyAgents;
			getNearbyAgents(nearbyAgents, p->mLoc, p->mVisualDistance);

			// Update agent's location
			p->update( mouseLoc, nearbyAgents, interfaceParams );

			// Ensure location is in bounds
			clampLocToTorus(p->mLoc);

			++p;
		}
	}

	// Reorganise binning
	sortAgentsIntoBins();
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
		//Vec2f loc = mouseLoc + Rand::randVec2f() * 5.0f; // random placement around mouse loc
		Vec2f loc = mouseLoc; // Non-random placement
		mAgents.push_back( Agent( loc, mAgents.size()+1 ) );
		allocateAgentToABin(&mAgents.back());
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
	Vec2f windowDim = Vec2f(screenWidth, screenHeight);
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
	vector<SpatialBin> newBins; 
	for(int i = 0; i < totalBins; i++){
		newBins.push_back(SpatialBin());
	}

	/*
	for( list<Agent>::iterator p = mAgents.begin(); p != mAgents.end(); p++) {
		
		if( p->mIsDead ){
			p = mAgents.erase( p );
		}
		else 
		{
			// Update agent's bin for new location
			int binNumber = convertLocToBin(p->mLoc);

			Agent * nextAgent = &*p;
			newBins[binNumber].addAgentToBin(&*p);
		}
	}
	*/

	for(int currentBinIndex = 0; currentBinIndex < totalBins; currentBinIndex++){
		
		int binnedAgents = gridBins[currentBinIndex].numBinnedAgents();
		list<Agent*>::iterator itrAgents = gridBins[currentBinIndex].getAgents();

		for(int i = 0; i < binnedAgents; i++){

			Agent * nextAgent = *itrAgents;
			// Check if the agent is allocated to the correct bin
			if(convertLocToBin(nextAgent->mLoc) != currentBinIndex){
				gridBins[currentBinIndex].erase(itrAgents);
			}

			// Increment the iterator
			itrAgents++;
		}
	}

	// Replace previous bin locations with new ones
	gridBins.swap(newBins);

	//delete newBins;

	//printBinContents();
}

void AgentController::getNearbyAgents(list<Agent*> &nearbyAgents, Vec3f centreLocation, float radius)
{
	// calculate number of surrounding bins required to search
	int radiusInBins = ceil(radius/binSize);
	// search area is a square of bins, search grid width is bin-length of side
	int searchGridWidth = (radiusInBins*2)+1;

	// iterate through columns and rows of search grid
	for (int i = -radiusInBins; i < radiusInBins+1; i++)
	{
		for (int j = -radiusInBins;  j < radiusInBins+1; j++)
		{
			int nextBinIndex = convertLocToBin(Vec3f(centreLocation.x + j*binSize, centreLocation.y+i*binSize, 0)); 
			// push pointers for all agents in this bin into nearbyAgents
			
			list<Agent*>::iterator agentItr = gridBins[nextBinIndex].getAgents();
			for(int nextAgentNumber = 0; nextAgentNumber < gridBins[nextBinIndex].numBinnedAgents(); nextAgentNumber++){
				nearbyAgents.push_back(*agentItr);
				agentItr++;
			}
		}
	}
}

int AgentController::convertLocToBin(Vec3f &loc)
{
	clampLocToTorus(loc);
	//console() << "locY: " << loc.y << ", locX: " << loc.x << std::endl;
	return (int)(floor(loc.y/binSize) * xBins) + (ceil(loc.x/binSize)-1);
}

// Given an agent, their location is used to obtain a binIndex and the agent pointer 
void AgentController::allocateAgentToABin(Agent * ptrAgent){
	
	int chosenBin = convertLocToBin(ptrAgent->mLoc);
	console() << "Chosen bin: " << chosenBin << std::endl;
	gridBins[convertLocToBin(ptrAgent->mLoc)].addAgentToBin(ptrAgent);
}

int AgentController::agentCount(){
	return (int)mAgents.size();
}

void AgentController::printBinContents(){
	console() << "Bin contents: " << std::endl;
	int binContents = -1;
	for(int i = 0; i < gridBins.size(); i++){
		binContents = gridBins[i].numBinnedAgents();
		if(binContents != 0){
			console() << i << " contains: " << binContents << std::endl;
		}
	}
}
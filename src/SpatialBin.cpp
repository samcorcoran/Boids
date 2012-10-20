#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "SpatialBin.h"
#include "Agent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

SpatialBin::SpatialBin(){

}

SpatialBin::SpatialBin(const int maxBinSize){
	m_binnedAgents = list<Agent*>(maxBinSize);
}

// Add passed agent to the list
void SpatialBin::addAgentToBin(Agent * nextAgent){
	m_binnedAgents.push_back(nextAgent);
}

// Return iterator over agents in this bin
list<Agent*>::iterator SpatialBin::getAgents(){
	return m_binnedAgents.begin();
}
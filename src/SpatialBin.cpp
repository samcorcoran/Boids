#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "SpatialBin.h"
#include "Agent.h"

using namespace ci;
using namespace ci::app;
using namespace std;

SpatialBin::SpatialBin(){

}

SpatialBin::SpatialBin(const int maxBinSize){
	m_binnedAgents = list<Agent*>(10);
}

// Add passed agent to the list
void SpatialBin::addAgent(Agent * nextAgent){
	m_binnedAgents.push_back(nextAgent);
}

// Return iterator over agents in this bin
list<Agent*>::iterator SpatialBin::getAgents(){
	return m_binnedAgents.begin();
}
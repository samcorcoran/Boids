#pragma once
#include <list>
#include <vector>

#include "Agent.h"
#include "InterfaceParams.h"

using namespace ci;
using namespace ci::app;
using std::list;
using std::vector;

class AgentController {
 public:
	AgentController();
	AgentController(const int binSize);
	void update( const ci::Vec2i &mouseLoc, InterfaceParams &interfaceParams );
	void draw();
	void addAgents( int amt, const ci::Vec2i &mouseLoc );
	void removeAgents( int amt );
	void clampLocToTorus(Vec3f &loc);
	void sortAgentsIntoBins();
	void getNearbyAgents(list<Agent*> &nearbyAgents, Vec3f centreLocation, float radius);
	int convertLocToBin(Vec3f &loc);

	list<Agent>	agents;

	// Spatial Binning variables
	vector<SpatialBin> * gridBins;

	int binSize;
	int xBins;
	int yBins;
	int totalBins;
	
};
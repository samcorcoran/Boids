#pragma once

#ifndef SPATIALBIN_H
#define SPATIALBIN_H

#include "Agent.h"
#include "cinder/Vector.h"
#include <list>
#include <vector>

using std::list;
using std::vector;

class SpatialBin {
public:
	// Constructor
	SpatialBin();
	SpatialBin::SpatialBin(const int maxBinSize);

	// Insert agents into list
	void addAgentToBin(Agent * nextAgent);
	// Return list for iteration
	list<Agent*>::iterator getAgents();

	int numBinnedAgents();

private:
	list<Agent*> m_binnedAgents;
};

#endif
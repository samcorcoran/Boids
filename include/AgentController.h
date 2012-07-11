#pragma once
#include "Agent.h"
#include <list>

using namespace ci;
using namespace ci::app;

class AgentController {
 public:
	AgentController();
	void update( const ci::Vec2i &mouseLoc,  const Vec4f &ruleWeights, const Vec3f &ruleRanges, const Vec3f &ruleSamples, const Vec3f &ruleCompatabilityThresholds );
	void draw();
	void addAgents( int amt, const ci::Vec2i &mouseLoc );
	void removeAgents( int amt );
	void clampLocToTorus(Vec3f &loc);

	std::list<Agent>	mAgents;
};
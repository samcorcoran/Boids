#pragma once

#ifndef AGENT_H
#define AGENT_H

#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include "InterfaceParams.h"
//#include "AgentController.h"

// These are declared in AgentController.h
//#include <list>
//#include <vector>

using namespace ci;
using namespace ci::app;

class Agent {
 public:
	Agent();
	Agent( ci::Vec2f loc, int newAgentId );
	void update( const ci::Vec2i &mouseLoc, std::list<Agent> &mAgents, InterfaceParams &interfaceParams);
	void draw();
	void drawVector(ci::Vec3f startLoc, ci::Vec3f vecToDraw, ci::Color chosenColor);
	void moveByVelocity();
	void turnTowardsHeading(ci::Vec2f desiredHeading);
	void rotate(ci::Vec2f &vec, double angle);
	void rotateAgentBy(double angle);
	void calculateNewHeading(std::list<Agent*> &mAgents, const ci::Vec2i &mouseLoc, InterfaceParams interfaceParams );
	ci::Vec3f toroidalVectorTo(const ci::Vec3f &start, const ci::Vec3f &end );
	void Agent::printAgentVectors();

	void collectNeighbouringAgents(std::list<Agent*> &neighbouringAgents);
	//friend void getNearbyAgents (AgentController);

	ci::Vec3f	mLoc;
	ci::Vec2f	mVel;
	
	int agentId;

	float		mMaxTurn;
	float		mDecay;
	float		mRadius;
	float		mScale;
	float		mVisualDistance;
	
	float		mMass;
	
	int			mAge;
	int			mLifespan;
	float		mAgePer;
	bool		mIsDead;

	ci::Vec2f lastSeparation;
	ci::Vec2f lastCohesion;
	ci::Vec2f lastAlignment;
	ci::Vec2f lastRand;
	ci::Vec2f lastNewHeading;
};

#endif
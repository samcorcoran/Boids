#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include <list>
#include <vector>

using namespace ci;

class Agent {
 public:
	Agent();
	Agent( ci::Vec2f loc, int newAgentId );
	void update( const Vec2i &mouseLoc, std::list<Agent> &mAgents, const Vec4f &ruleWeights, const Vec3f &ruleRanges, const Vec3f &ruleSamples, const Vec3f &ruleCompatabilityThresholds );
	void draw();
	void drawVector(Vec3f startLoc, Vec3f vecToDraw, Color chosenColor);
	void turnTowardsHeading(Vec2f desiredHeading);
	void rotate(Vec2f &vec, double angle);
	void rotateAgentBy(double angle);
	void calculateNewHeading(std::list<Agent> &mAgents, const Vec2i &mouseLoc, const Vec4f &ruleWeights, const Vec3f &ruleRanges, const Vec3f &ruleSamples, const Vec3f &ruleCompatabilityThresholds );
	Vec3f toroidalVectorTo(const Vec3f &start, const Vec3f &end );
	void Agent::printAgentVectors();

	ci::Vec3f	mLoc;
	ci::Vec2f	mVel;
	
	int agentId;

	float		mMaxTurn;
	float		mDecay;
	float		mRadius;
	float		mScale;
	
	float		mMass;
	
	int			mAge;
	int			mLifespan;
	float		mAgePer;
	bool		mIsDead;

	Vec2f lastSeparation;
	Vec2f lastCohesion;
	Vec2f lastAlignment;
	Vec2f lastRand;
	Vec2f lastNewHeading;
};

#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/CinderMath.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

#include "Agent.h"
#include "InterfaceParams.h"
//#include "AgentController.h"

//#include <list>
#include <cmath>
//#include <vector>

using namespace ci;
using namespace ci::app;
using std::list;

Agent::Agent()
{
}

Agent::Agent( Vec2f loc, int newAgentId )
{
	mLoc			= Vec3f(loc, 0.0f);
	console() << "Loc: " << mLoc << std::endl;
	mVel			= Rand::randVec2f() * 2.5;

	agentId			= newAgentId;

	mMaxTurn		= toRadians(5.0f);
	mDecay			= Rand::randFloat( 0.96f, 0.99f );
	mRadius			= 2.0f;
	mScale			= 3.0f;
	mAge			= 0;
	mLifespan		= Rand::randInt( 50, 250 );
	mAgePer			= 1.0f;
	mIsDead			= false;
	mVisualDistance = 50.0f;

	lastSeparation = Vec2f::zero();
	lastCohesion = Vec2f::zero();
	lastAlignment = Vec2f::zero();
	lastRand = Vec2f::zero();
	lastNewHeading = Vec2f::zero();
}	

void Agent::update( const ci::Vec2i &mouseLoc, std::list<Agent*> &agentList, InterfaceParams &interfaceParams )
{	
	//console() << "Agent " << this->agentId << " has possible neighbours: " << agentList.size() << "!"  << std::endl;
	calculateNewHeading(agentList, mouseLoc, interfaceParams);
	moveByVelocity();
}

void Agent::draw()
{
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	
	float arrowLength = mVel.length() + 2;
	float headLength = 3.0f;
    float headRadius = 2.5f;

	Vec3f p1 = mLoc;
    Vec3f p2(Vec2f(mLoc.x, mLoc.y) + (mVel.normalized()*arrowLength));
	
	//gl::drawSolidCircle(Vec2f(mLoc.x, mLoc.y),2.0f);
	gl::drawVector(p1, p2, headLength, headRadius );
	
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
}

void Agent::moveByVelocity(){
	mLoc.x += mVel.x;
	mLoc.y += mVel.y;
}

void Agent::drawVector(ci::Vec3f startLoc, ci::Vec3f vecToDraw, Color chosenColor)
{
	if (vecToDraw.length() != 0)
	{
		float scaleFactor = 8.0f;
		gl::color( chosenColor );
		float headLength = 3.0f;
		float headRadius = 3.0f;
		//console() << "Drawing vec: " << vecToDraw << ", fromTo "<< startLoc << " " << startLoc+vecToDraw << ", len: " << vecToDraw.length() << std::endl;
		gl::drawVector(startLoc, startLoc+(vecToDraw*scaleFactor), headLength, headRadius );
	}
}

void Agent::drawCASInfluenceVectors(bool drawCohesion, bool drawAlignment, bool drawSeparation)
{
	// Draw cohesion vector
	if(drawCohesion) drawVector(mLoc, Vec3f(lastCohesion, 0.0f), Color( 1.0f, 0.0f, 0.0f ) );
		
	// Draw alignment vector		
	if(drawAlignment) drawVector(mLoc, Vec3f(lastAlignment, 0.0f), Color( 0.0f, 1.0f, 0.0f ) );
	
	// Draw separation vector
	if(drawSeparation) drawVector(mLoc, Vec3f(lastSeparation, 0.0f), Color( 1.0f, 0.0f, 1.0f ) );
}

// Adjusts mVel in direction of desired heading to as great a degree as maxTurn allows
void Agent::turnTowardsHeading(ci::Vec2f desiredHeading)
{
	//console() << "mVel: " << mVel << "  desired: " << desiredHeading << std::endl;
	
	float angleBetween =  atan2(desiredHeading.y, desiredHeading.x) - atan2(mVel.y, mVel.x);
		
	// This is meant to help avoid turning the longer way round...
	if ( angleBetween > (M_PI) ) {
		//console() << "Subtracted pi from desired heading: " << angleBetween << ", " << angleBetween - M_PI << std::endl;
		angleBetween -= 2 * M_PI;
	} 
	else if ( angleBetween < -(M_PI) ) {
		//console() << "Added pi from desired heading: " << angleBetween << ", " << angleBetween + M_PI << std::endl;
		angleBetween += 2 * M_PI;
	}


	if (angleBetween > mMaxTurn) 
	{
		//console() << "angleBetween > maxTurn (" << angleBetween << " , " << mMaxTurn << ")" << std::endl;
		rotateAgentBy(mMaxTurn);
	}
	else if (angleBetween < -mMaxTurn) {
		//console() << "angleBetween < -maxTurn (" << angleBetween << " , " << mMaxTurn << ")" << std::endl;
		rotateAgentBy(-mMaxTurn);
	}
	else{
		//console() << "angleBetween was acceptable (" << angleBetween << ")" << std::endl;
		rotateAgentBy(angleBetween);
	}
}

void Agent::rotate(ci::Vec2f &vec, double angle)
{
	ci::Vec2f newVec;

	float cosa = cos( angle );
	float sina = sin( angle );

	newVec.x = (vec.x * cosa) - (vec.y * sina);
	newVec.y = (vec.x * sina) + (vec.y * cosa);

	vec = newVec;
}

void Agent::rotateAgentBy(double angle)
{
	Vec2f newVec;

	float cosa = cos( angle );
	float sina = sin( angle );

	newVec.x = (mVel.x * cosa) - (mVel.y * sina);
	newVec.y = (mVel.x * sina) + (mVel.y * cosa);

	mVel = newVec;
	//app::console() << "New vel: " << mVel << std::endl;
}

void Agent::calculateNewHeading(std::list<Agent*> &neighbourAgents, const ci::Vec2i &mouseLoc, InterfaceParams interfaceParams ){

	// Sum of Weights
	float totalWeights = interfaceParams.mAlignmentWeight + interfaceParams.mCohesionWeight + interfaceParams.mCohesionWeight + interfaceParams.mRandWeight;

	// Random Heading
	float randWeight = interfaceParams.mRandWeight / totalWeights;
	ci::Vec2f randHeading = mVel;
	if (Rand::randFloat() < 0.07) { 
		rotate(randHeading, Rand::randFloat(toRadians(-10.0f), toRadians(10.0f)));
	}
	
	// Cohesion heading
	ci::Vec2f cohesionHeading = Vec2f::zero();
	float cohWeight = interfaceParams.mCohesionWeight / totalWeights;
	float cohesionDistance = interfaceParams.mCohesionRange;
	float cohesionSamples = interfaceParams.mCohesionSamples;
	float cohesionCompatabilityThresh = interfaceParams.mCohesionCompatabilityThresh;

	// Alignment heading
	ci::Vec2f alignmentHeading = ci::Vec2f::zero();
	float aliWeight = interfaceParams.mAlignmentWeight / totalWeights;
	float alignmentDistance = interfaceParams.mAlignmentRange;
	float alignmentnSamples = interfaceParams.mAlignmentSamples;
	float alignmentCompatabilityThresh = interfaceParams.mAlignmentCompatabilityThresh;

	// Separation heading
	ci::Vec2f separationHeading = ci::Vec2f::zero();
	float sepWeight = interfaceParams.mSeparationWeight / totalWeights;
	float separationDistance = interfaceParams.mSeparationRange;
	float separationSamples = interfaceParams.mSeparationSamples;
	float separationCompatabilityThresh = interfaceParams.mSeparationCompatabilityThresh;

	// proportion of behind agent which will be ignored
	float rearIgnoranceArc = 0.1;

	// influence counters
	int cohesionPartners = 0;
	int alignmentPartners = 0;
	int separationPartners = 0;

	// Base maximum visual range on largest steering influence range
	float visualRange = 10;
	if (alignmentDistance > visualRange) visualRange = alignmentDistance;
	if (cohesionDistance > visualRange) visualRange = cohesionDistance;
	if (separationDistance > visualRange) visualRange = separationDistance;

	Agent * nextNeighbour;
	for( list<Agent*>::iterator p = neighbourAgents.begin(); p != neighbourAgents.end(); ++p )
	{
		nextNeighbour = *p;

		// Check for co-location, so agent can ignore self
		if ( nextNeighbour->mLoc != mLoc )
		{
			// Retrieve heading to nextAgent
			ci::Vec3f vecToNextAgent = ci::Vec3f::zero();
			//vecToNextAgent = toroidalVectorTo(mLoc, nextNeighbour->mLoc);	
			vecToNextAgent = nextNeighbour->mLoc - mLoc;	

			// Exclude agents behind this one
			float exclude = (mVel.safeNormalized().dot(Vec2f(vecToNextAgent.x, vecToNextAgent.y).safeNormalized())+1);
			//console() << "EXCLUDE = " << exclude << std::endl;
			if (exclude > rearIgnoranceArc)
			{
				// Calculate similarity between self and nextAgent
				float vecSimilarity = (mVel.safeNormalized().dot((nextNeighbour->mVel).safeNormalized()) + 1) / 2;

				// Check nextAgent's heading is similar enough for cohesion and near enough
				if (vecSimilarity > cohesionCompatabilityThresh && vecToNextAgent.length() < cohesionDistance )
				{ 
					cohesionHeading = cohesionHeading + ci::Vec2f(vecToNextAgent.x, vecToNextAgent.y);
					cohesionPartners++;
				}
				else {
					//console() << "Agent " << agentId << " rejected a cohesion sample." << std::endl;
				}

				// Check nextAgent's heading is similar enough for alignment and near enough
				if (vecSimilarity > alignmentCompatabilityThresh &&	vecToNextAgent.length() < alignmentDistance )
				{ 
					// Normalize influence heading so fast movers are not prioritised
					alignmentHeading = alignmentHeading + (nextNeighbour->mVel).safeNormalized();
					alignmentPartners++;
				}
				else {
					//console() << "Agent " << agentId << " rejected a alignment sample." << std::endl;
				}

				// Check nextAgent's heading is similar enough for separation and near enough
				if (vecSimilarity > separationCompatabilityThresh && vecToNextAgent.length() < separationDistance )
				{ 
					// x and y are negated to get opposite heading
					separationHeading = separationHeading - ((Vec2f(vecToNextAgent.x, vecToNextAgent.y)) * (vecToNextAgent.length()/separationDistance));
					separationPartners++;
					//console() << "Agent " << agentId << " found a separation sample!" << std::endl;
				}
				else {
					//console() << "Agent " << agentId << " rejected a separation sample." << std::endl;
				}
			}
			else{
				//console() << "Agent " << agentId << " excluded an agent behind it." << std::endl;
			}
		}
	}

	// Average out influences based on partner totals
	// Avoid divide by zero
	if (cohesionPartners > 0) 
	{
		cohesionHeading = cohesionHeading / cohesionPartners;
	}
	if (alignmentPartners > 0) 
	{
		alignmentHeading = alignmentHeading / alignmentPartners;
	}
	if (separationPartners > 0) 
	{
		separationHeading = separationHeading / separationPartners;
	}

	// Mouse predator
	//Vec2f mouseSeparation = Vec2f::zero();
	//float mouseSeparationWeight = 0.0f;
	//Vec2f vecToMouse = mouseLoc - Vec2f(mLoc.x, mLoc.y);
	//if (vecToMouse.length() < 100)
	//{
	//	mouseSeparation = mouseSeparation - Vec2f((mouseLoc.x - mLoc.x), (mouseLoc.y - mLoc.y));
	//	mouseSeparationWeight = 50.0f;
	//}

	Vec2f newHeading = Vec2f::zero();

	// Determine if a change in heading is necessary
	if (totalWeights != 0)
	{
		/*
		// WHY DOES RAND Weight of zero make everyone default to horizontal movement?
		newHeading =		(randHeading.safeNormalized()			* randWeight)
						+	(cohesionHeading.safeNormalized()		* cohWeight)
						+	(alignmentHeading.safeNormalized()		* aliWeight)
						+	(separationHeading.safeNormalized()		* sepWeight);
		*/


		randHeading = randHeading.safeNormalized() * randWeight;
		cohesionHeading = cohesionHeading.safeNormalized() * cohWeight;
		alignmentHeading = alignmentHeading.safeNormalized() * aliWeight;
		separationHeading = separationHeading.safeNormalized() * sepWeight;

		lastRand = randHeading;
		lastCohesion = cohesionHeading;
		lastAlignment = alignmentHeading;
		lastSeparation = separationHeading;
		
		newHeading = randHeading + cohesionHeading + alignmentHeading + separationHeading;
		lastNewHeading = newHeading;

		//printAgentVectors();

		//console() << "current heading: " << mVel << ", newHeading: " << newHeading << std::endl;

		if (newHeading.x != 0 && newHeading.y != 0)
		{
			turnTowardsHeading(newHeading);
		}
	}
	else{
		console() << "totalWeights was zero." << std::endl;
	}
}

ci::Vec3f Agent::toroidalVectorTo(const ci::Vec3f &start, const ci::Vec3f &end){
	// Accomodate toroidal shape
	
	Vec3f vecToEnd = end - start;
	//console() << "Initial vecToEnd before toroidal fix: " << vecToEnd << std::endl;
	// Handle x-edges
	if (vecToEnd.x > 400)
	{
		//console() << "Friend was x>400 away, subtracted 800." << std::endl;
		// Over half the world away, in positive x, so must be less than half in negative x
		vecToEnd.x = vecToEnd.x - 800;
	}
	else if((vecToEnd.x) < -400)
	{
		//console() << "Friend was x<-400 away, added 800." << std::endl;
		// Over half the world away, in negative x, so must be less than half in positive x
		vecToEnd.x = vecToEnd.x + 800;
	}
	
	// Handle y-edges
	if (vecToEnd.y > 300)
	{
		//console() << "Friend was y>300 away, subtracted 600." << std::endl;
		// Over half the world away, in positive y, so must be less than half in negative y
		vecToEnd.y = vecToEnd.y - 600;
	}
	else if(vecToEnd.y < -300)
	{
		//console() << "Friend was y<-300 away, added 600." << std::endl;
		// Over half the world away, in negative y, so must be less than half in positive y
		vecToEnd.y = vecToEnd.y + 600;
	}

	//console() << "Resulting vecToEnd: " << vecToEnd << std::endl;
	
	return vecToEnd;
}

void Agent::collectNeighbouringAgents(std::list<Agent*> &neighbouringAgents){
	//getNearbyAgents(neighbouringAgents, this->mLoc, this->mVisualDistance);
}

void Agent::printAgentVectors(){

	console() << "Agent " << agentId << ": " << std::endl;
	console() << " mLoc: " << mLoc << std::endl;
	console() << " mVel: " << mVel << ", len: "<< mVel.length() << std::endl;
	console() << " rand: " << lastRand << ", " << lastRand.length() << std::endl;
	console() << " cohesion: " << lastCohesion << ", " << lastCohesion.length() << std::endl;
	console() << " alignment: " << lastAlignment << ", " << lastAlignment.length() << std::endl;
	console() << " separation: " << lastSeparation << ", " << lastSeparation.length() << std::endl;
	console() << " newHeading: " << lastSeparation << ", " << lastSeparation.length() << std::endl;
	console() << " ----- " << std::endl;
}
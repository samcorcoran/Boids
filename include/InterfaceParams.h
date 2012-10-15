#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Perlin.h"
#include <list>
#include <vector>

// Holds parameters that are altered by the ui panel, so values can be easily passed to other functions
struct InterfaceParams {
	float mSeparationWeight;
	float mAlignmentWeight;
	float mCohesionWeight;
	float mRandWeight;

	float mSeparationRange;
	float mAlignmentRange;
	float mCohesionRange;
	
	float mSeparationSamples;
	float mAlignmentSamples;
	float mCohesionSamples;

	float mSeparationCompatabilityThresh;
	float mAlignmentCompatabilityThresh;
	float mCohesionCompatabilityThresh;

	bool drawAgents;
}
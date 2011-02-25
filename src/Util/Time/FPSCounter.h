/*
 * FPSCounter.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include <list>

namespace Flewnit
{

class Timer;

class FPSCounter
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	Timer* mTimer;

	int mNumTotalFrames;

	unsigned int mNumFramesToAverage;

	bool mFrameEndedWasCalledGuard;

	std::list<double> mLastFrameDurations;

public:
	FPSCounter( int framesToAverage = 20);
	virtual ~FPSCounter();

	void setFramesToAverage(unsigned int value);

	void newFrameStarted();
	void frameEnded();

	double getFPS(bool averaged = false);
	double getLastFrameDuration()const;
	int getTotalRenderedFrames()const;
};

}


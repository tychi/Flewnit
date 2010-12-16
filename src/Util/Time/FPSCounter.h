/*
 * FPSCounter.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include <queue>

namespace Flewnit
{

#include "Timer.h"

class FPSCounter
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	Timer* mTimer;

	int mNumTotalFrames;

	int mNumFramesToAverage;

	bool mFrameEndedWasCalledGuard;

	std::queue<double> mLastFrameDurations;

public:
	FPSCounter( int framesToAverage = 20);
	virtual ~FPSCounter();

	void setFramesToAverage(int value);

	void newFrameStarted();
	void frameEnded();

	double getFPS(bool averaged = false);
	int getTotalRenderedFrames();
};

}


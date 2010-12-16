/*
 * GLFWTimer.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#pragma once

#include "Timer.h"

#ifdef FLEWNIT_USE_GLFW

namespace Flewnit
{

class GLFWTimer : public Timer
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	//construction only by Timer::create();
	GLFWTimer();

	virtual ~GLFWTimer();

	virtual void getCurrentTime();

	virtual void start();
	virtual void stop();
	virtual void reset();



	virtual double getElapsedTimeInSecondsDouble();
	virtual float getElapsedTimeInSecondsFloat();

private:
	bool mIsRunning;

	double mLastStartTime;
	double mTotalElapsedNonPausedTime;
};

}

#endif // FLEWNIT_USE_GLFW

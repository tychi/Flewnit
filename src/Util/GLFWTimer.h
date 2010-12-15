/*
 * GLFWTimer.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#pragma once

#include "Timer.h"

namespace Flewnit
{

class GLFWTimer : public Timer
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	//construction only by Timer::create();
	GLFWTimer();

	virtual ~GLFWTimer();

	virtual void start();
	virtual void pause();
	virtual void resume();
	virtual void stop();



	virtual float getElapsedTimeInMilliSecondsFloat();
	virtual float getElapsedTimeInMicroSecondsFloat();

	virtual int getElapsedTimeInMilliSecondsInt();
	virtual int getElapsedTimeInMicroSecondsInt();
};

}

/*
 * Timer.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include "Common/BasicObject.h"

namespace Flewnit
{

class Timer : public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
protected:

	Timer(){}

public:

	//instancing function in order to delegate the instantiation of the concrete Timer class (depends on WindowManager)
	static Timer* create();

	virtual ~Timer(){}

	virtual double getCurrentTime()=0;

	virtual void start()=0;
	virtual void stop()=0;
	virtual void reset()=0;

	virtual double getElapsedTimeInSecondsDouble()=0;
	virtual float getElapsedTimeInSecondsFloat()=0;


};

}


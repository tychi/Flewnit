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
	virtual ~Timer(){}

public:
	//instancing function in order to delegate the instantiation of the concrete Timer class (depends on WindowManager)
	static Timer* create();


	virtual void start()=0;
	virtual void pause()=0;
	virtual void resume()=0;
	virtual void stop()=0;



	virtual float getElapsedTimeInMilliSecondsFloat()=0;
	virtual float getElapsedTimeInMicroSecondsFloat()=0;

	virtual int getElapsedTimeInMilliSecondsInt()=0;
	virtual int getElapsedTimeInMicroSecondsInt()=0;
};

}


/*
 * InputDevice.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

namespace Flewnit
{

class InputInterpreter;
class Timer;

class InputDevice
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	double mTimeAtLastUpdate;
	double mTimeSinceLastUpdate;
	Timer* mTimer;

public:
	InputDevice();
	virtual ~InputDevice();

	//partial visitor pattern : call "visit" part actively, i.e. without explicit accept-part; Visitor is InputInterpreter, and it is acquired from the InputManager
	virtual void notifyInterpreter()=0;

};

}


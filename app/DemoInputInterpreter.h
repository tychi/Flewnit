/*
 * DemoInputInterpreter.h
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#pragma once

#include "UserInterface/Input/InputInterpreter.h"

namespace Flewnit
{

class DemoInputInterpreter: public Flewnit::InputInterpreter
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	DemoInputInterpreter(float cameraLookMouseSensivity =1.0f);
	virtual ~DemoInputInterpreter();


	//"partial visitor pattern": kind of virtual callback: Devices call these functions from their own callback functions, if they have some;
	virtual void interpretInput(Keyboard* keyboard);
	virtual void interpretInput(Mouse* mouse);
	virtual void interpretInput(WiiMote* wiiMote);

private:

	float mCameraLookMouseSensivity;
};

}


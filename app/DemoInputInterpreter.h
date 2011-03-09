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
	DemoInputInterpreter(float cameraLookMouseSensivity =1.0f, float movementSpeed=50.0f /*0.033f*/);
	virtual ~DemoInputInterpreter();

	//if no input is done, the interpretInput methods aren't called; to do stuff like continous
	//update of certain variables, this method is called after every frame
	virtual void perFrameCallback();


	//"partial visitor pattern": kind of virtual callback: Devices call these functions from their own callback functions, if they have some;
	virtual void interpretInput(Keyboard* keyboard);
	virtual void interpretInput(Mouse* mouse);
	virtual void interpretInput(WiiMote* wiiMote);

private:

	float mCameraLookMouseSensivity;
	float mMovementSpeed;

	enum MovementDirections
	{
		MOVEMENT_DIRECTION_FRONT_BACK=0,
		MOVEMENT_DIRECTION_RIGHT_LEFT=1,
		MOVEMENT_DIRECTION_UP_DOWN=2,
		__NUM_MOVEMENT_DIRECTIONS__= 3
	};

	enum MovementState
	{
//		FORWARD= 1,
//		STILL =0,
//		BACKWARD=-1
		MOVEMENT_STATE_FORWARD= 0,
		MOVEMENT_STATE_STILL =1,
		MOVEMENT_STATE_BACKWARD=2
	};

	MovementState mMovementState[__NUM_MOVEMENT_DIRECTIONS__];
	bool mHideMouse;
};

}


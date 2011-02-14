/*
 * InputInterpreter.h
 *
 *  Created on: Dec 15, 2010
 *      Author: tychi
 *
 *  Library Users derive from this class their application logic implementation;
 */

#pragma once

#include "Common/BasicObject.h"



namespace Flewnit
{

class Keyboard;
class Mouse;
class WiiMote;


class InputInterpreter
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;


public:
	InputInterpreter();
	virtual ~InputInterpreter();

	//is called by InputManager after registration;
	//virtual void init()=0;


	//"partial visitor pattern": kind of virtual callback: Devices call these functions from their own callback functions, if they have some;
	virtual void interpretInput(Keyboard* keyboard)=0;
	virtual void interpretInput(Mouse* mouse)=0;
	virtual void interpretInput(WiiMote* wiiMote)=0;

	//if no input is done, the interpretInput methods aren't called; to do stuff like continous
	//update of certain variables, this method is called after every frame
	virtual void perFrameCallback()=0;

};

}


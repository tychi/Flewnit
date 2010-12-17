/*
 * Keyboard.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "InputDevice.h"

namespace Flewnit
{

class Keyboard
:public InputDevice
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	Keyboard(): mRecentKey(0), mRecentStatus(0){}
	virtual ~Keyboard();



	//inputmanager will call this function when it receives input events (e.g. from the window manager, e.g. GLFW);
	void keyPressed(int value, int status)
	{
		mRecentKey = value;
		mRecentStatus = status;
		notifyInterpreter();
	}


	virtual void notifyInterpreter();

	int getRecentKey(){return mRecentKey;}
	int getRecentStatus(){return mRecentStatus;}

private:

	int mRecentKey;
	int mRecentStatus;

};

}


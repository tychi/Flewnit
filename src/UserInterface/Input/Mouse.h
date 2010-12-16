/*
 * Mouse.h
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#pragma once

#include "InputDevice.h"

#include "Common/Math.h"

namespace Flewnit
{

class Mouse
:public InputDevice
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	Mouse(): mRecentButton(0), mRecentStatus(0){}
	virtual ~Mouse();





	//inputmanager will call this function when it receives input events (e.g. from the window manager, e.g. GLFW);
	void buttonPressed(int value, int status)
	{
		mRecentButton = value;
		mRecentStatus = status;
		notifyInterpreter();
	}

	void positionChanged(Vector2Di const& newPos)
	{

	}



	virtual void notifyInterpreter();

	int getRecentButton(){return mRecentButton;}
	int getRecentStatus(){return mRecentStatus;}

private:

	int mRecentButton;
	int mRecentStatus;

	Vector2Di mRecentPosition;
	Vector2Di mLastPosition;

};

}

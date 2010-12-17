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

	enum MouseEvent
	{
		MOUSE_EVENT_NONE,
		MOUSE_EVENT_POSITION_CHANGED,
		MOUSE_EVENT_BUTTON_CHANGED
	};

	Mouse(): mRecentButton(0), mRecentStatus(0), mLastPosition(0,0),mRecentPosition(0,0), mRecentEvent(MOUSE_EVENT_NONE)
	{}

	virtual ~Mouse();






	//inputmanager will call this function when it receives input events (e.g. from the window manager, e.g. GLFW);
	void buttonChanged(int value, int status)
	{
		mRecentButton = value;
		mRecentStatus = status;
		mRecentEvent= MOUSE_EVENT_BUTTON_CHANGED;
		notifyInterpreter();
	}

	void positionChanged(Vector2Di const& newPos)
	{
		mLastPosition = mRecentPosition;
		mRecentPosition = newPos;

		mRecentEvent= MOUSE_EVENT_POSITION_CHANGED;
		notifyInterpreter();
	}



	virtual void notifyInterpreter();

	int getRecentButton(){return mRecentButton;}
	int getRecentStatus(){return mRecentStatus;}


	inline MouseEvent getRecentEvent()const{return mRecentEvent;}

private:

	int mRecentButton;
	int mRecentStatus;

	Vector2Di mRecentPosition;
	Vector2Di mLastPosition;

	MouseEvent mRecentEvent;

};

}

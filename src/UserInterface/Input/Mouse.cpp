/*
 * Mouse.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Mouse.h"

#include "URE.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "UserInterface/Input/InputInterpreter.h"

namespace Flewnit
{



Mouse::~Mouse()
{
	// TODO Auto-generated destructor stub
}

void Mouse::notifyInterpreter()
{
	if(URE_INSTANCE -> getInputInterpreter())
	{
		URE_INSTANCE->getInputInterpreter()->interpretInput(this);
	}
}


bool Mouse::isHidden()const
{
	return mIsHidden;
}

void Mouse::setHidden(bool value)
{
	mIsHidden = value;
	//forward request to window manager;
	WindowManager::getInstance().setMouseGrab(value);
}


}

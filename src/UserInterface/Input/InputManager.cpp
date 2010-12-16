/*
 * InputManager.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "InputManager.h"
#include "Util/Log/Log.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "WiiMote.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

InputManager::InputManager()
: mInputInterpreter(0)
{
	// TODO Auto-generated constructor stub

}

InputManager::~InputManager()
{
	delete mKeyboard;
	delete mMouse;
	delete mWiiMote;
}




//let die InputInterpreter do his work: set camera, delegate GUI
void InputManager::processInput()
{
	assert(mInputInterpreter);

	BOOST_FOREACH(InputDevice* idev, mInputDevices)
	{

	}

}

void InputManager::setInputInterpreter(InputInterpreter* inputInterpreter)
{
	if(mInputInterpreter)
	{
		LOG<< WARNING_LOG_LEVEL << "InputInterpreter already set; deleting old one ;\n";
	}
	mInputInterpreter = inputInterpreter;
}

}

/*
 * Keyboard.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Keyboard.h"

#include "URE.h"
#include "UserInterface/WindowManager/WindowManager.h"
#include "UserInterface/Input/InputInterpreter.h"



namespace Flewnit
{

Keyboard::~Keyboard()
{
	// TODO Auto-generated destructor stub
}


void Keyboard::notifyInterpreter()
{
	if(URE_INSTANCE -> getInputInterpreter())
	{
		URE_INSTANCE->getInputInterpreter()->interpretInput(this);
	}
}




}

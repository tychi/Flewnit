/*
 * Keyboard.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Keyboard.h"

#include "URE.h"
#include "UserInterface/Input/InputInterpreter.h"

namespace Flewnit
{





Keyboard::~Keyboard()
{
	// TODO Auto-generated destructor stub
}


void Keyboard::notifyInterpreter()
{
	URE_INSTANCE->getWindowManager()->getInputInterpreter()->interpretInput(this);
}




}

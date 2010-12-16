/*
 * Mouse.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "Mouse.h"

namespace Flewnit
{



Mouse::~Mouse()
{
	// TODO Auto-generated destructor stub
}

void Mouse::notifyInterpreter()
{
	URE_INSTANCE->getWindowManager()->getInputInterpreter()->interpretInput(this);
}


}

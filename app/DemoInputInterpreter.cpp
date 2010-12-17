/*
 * DemoInputInterpreter.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "DemoInputInterpreter.h"
#include "Util/Log/Log.h"

#include "URE.h"

#ifdef FLEWNIT_USE_GLFW
#	include <GL/glfw.h>
#else

#endif

namespace Flewnit
{

DemoInputInterpreter::DemoInputInterpreter()
{
	// TODO Auto-generated constructor stub

}

DemoInputInterpreter::~DemoInputInterpreter()
{
	// TODO Auto-generated destructor stub
}

void DemoInputInterpreter::interpretInput(Keyboard* keyboard)
{
	if(keyboard->getRecentKey() == GLFW_KEY_ENTER)
	{
		LOG<<DEBUG_LOG_LEVEL<<"enter pressed!!11 ;(";
	}

	if(keyboard->getRecentKey() == GLFW_KEY_ESC)
	{
		LOG<<DEBUG_LOG_LEVEL<<"escape pressed, shutting down;(";
		URE_INSTANCE->requestMainLoopQuit();
	}
}

void DemoInputInterpreter::interpretInput(Mouse* mouse)
{

}

void DemoInputInterpreter::interpretInput(WiiMote* wiiMote)
{
	//nothing to do yet;
}

}

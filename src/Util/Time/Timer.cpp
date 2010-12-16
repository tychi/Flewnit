/*
 * Timer.cpp
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 */

#include "Timer.h"

#include "Util/Log/Log.h"

#include <assert.h>

#ifdef FLEWNIT_USE_GLFW
#	include "GLFWTimer.h"
#else

#endif


namespace Flewnit
{



Timer* Timer::create()
{
#ifdef FLEWNIT_USE_GLFW
	return new GLFWTimer();
#else
	LOG<< ERROR_LOG_LEVEL << "Timer::create(): Sorry, no other Timer but a GLFW-one is currently implemented."
	assert(0);
#endif
}



}


/*
 * GLFWTimer.cpp
 *
 *  Created on: Dec 15, 2010
 *      Author: tychi
 */

#include "GLFWTimer.h"

#ifdef FLEWNIT_USE_GLFW

#include <GL/glfw.h>

//need this to assert glfw has bin intialized;
#	include "URE.h"


namespace Flewnit
{

GLFWTimer::GLFWTimer()
{
	// TODO Auto-generated constructor stub
	assert("Sorry for the inconvenence, but GLFW must be initialized before a timer can be used :( Bad haxx as we use glfwGetTime() internally ;("
			&& URE_INSTANCE->getWindowManager());

	reset();

}

GLFWTimer::~GLFWTimer()
{
	// TODO Auto-generated destructor stub
}


virtual void GLFWTimer::getCurrentTime()
{
	return glfwGetTime();
}


void GLFWTimer::start()
{
	mLastStartTime = glfwGetTime();

	mIsRunning =true;
}



void GLFWTimer::stop()
{
	if(mIsRunning)
	{
		mTotalElapsedNonPausedTime += (glfwGetTime() - mLastStartTime);
	}


	mIsRunning = false;
}




void GLFWTimer::reset()
{
	mIsRunning =false;

	mLastStartTime =0.0;
	mTotalElapsedNonPausedTime =0.0;
}



double GLFWTimer::getElapsedTimeInSecondsDouble()
{
	return mIsRunning ?
			//do exact measuring, don't just return the last value after stop() is called; One might wanna query stuff while long-term- profiling ;(
			mTotalElapsedNonPausedTime + (glfwGetTime() - mLastStartTime)
			:
			mTotalElapsedNonPausedTime
			;
}

float GLFWTimer::getElapsedTimeInSecondsFloat()
{
	return static_cast<float>(getElapsedTimeInSecondsDouble());
}

}

#endif //FLEWNIT_USE_GLFW

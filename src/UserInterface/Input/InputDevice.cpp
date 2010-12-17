/*
 * InputDevice.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: tychi
 */

#include "InputDevice.h"
#include "Util/Time/Timer.h"
#include "Util/Log/Log.h"

namespace Flewnit
{

InputDevice::InputDevice()
{

//	mTimer = Timer::create();
//
//	mTimeAtLastUpdate = mTimer->getCurrentTime();
//	mTimeSinceLastUpdate = 0.0;

}

InputDevice::~InputDevice()
{
	//delete mTimer;
}

//void InputDevice::updateTime()
//{
//	mTimeSinceLastUpdate = mTimer->getCurrentTime() - mTimeAtLastUpdate;
//	mTimeAtLastUpdate = mTimer->getCurrentTime();
//
//	//catch the case that between init of URE and first update of a IntpuDevice passed a lot of time, which would degenerate input processing;
//	if(mTimeSinceLastUpdate > FLEWNIT_NON_RESPONSIVE_WARNING_TIME)
//	{
//		LOG<<WARNING_LOG_LEVEL<<"InputDevice::updateTime(): Time since last upadet is "<<mTimeSinceLastUpdate<<"; It will be clamped to "
//				<< FLEWNIT_NON_RESPONSIVE_WARNING_TIME <<" in order to provide \n";
//
//		mTimeSinceLastUpdate = FLEWNIT_NON_RESPONSIVE_WARNING_TIME;
//	}
//}

}

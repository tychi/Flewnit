/*
 * FPSCounter.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "FPSCounter.h"

#include "Timer.h"

#include <boost/foreach.hpp>
#include "Util/Log/Log.h"

namespace Flewnit
{


FPSCounter::FPSCounter(int framesToAverage)
: 	mTimer(Timer::create()),
  	mNumTotalFrames(0),
	mNumFramesToAverage(framesToAverage),
	mFrameEndedWasCalledGuard(true)
{

	// TODO Auto-generated constructor stub

}

FPSCounter::~FPSCounter()
{
	delete mTimer;
	// TODO Auto-generated destructor stub
}


void FPSCounter::setFramesToAverage(unsigned int value)
{
	//no sophisticatede adjustmenst stuff here, hard reset ;)
	mLastFrameDurations.empty();
	mNumFramesToAverage = value;
}

void FPSCounter::newFrameStarted()
{
	if(!mFrameEndedWasCalledGuard)
	{
		frameEnded();
	}


	mTimer->reset();

	mTimer->start();
	mFrameEndedWasCalledGuard = false;
}

void FPSCounter::frameEnded()
{
	mTimer->stop();
	mNumTotalFrames++;

	//add last frame duration to queue;
	mLastFrameDurations.push_back( mTimer->getElapsedTimeInSecondsDouble());

	if(mLastFrameDurations.size() >  mNumFramesToAverage )
	{
		//delete first element, so that we only keep track of the recent mNumFramesToAverage values;
		mLastFrameDurations.pop_front();
	}

	mFrameEndedWasCalledGuard=true;
}

double FPSCounter::getFPS(bool averaged)
{
//	assert("We need at least one frame passed in order to obtain a valid FPS value;"
//			&& !mLastFrameDurations.empty() && mLastFrameDurations.back()>0.0);

	if( mLastFrameDurations.empty() || mLastFrameDurations.back()<=0.0)
	{
		LOG<<WARNING_LOG_LEVEL<<" FPSCounter::getFPS();We need at least one frame passed in order to obtain a valid FPS value;\n";
		return 0.0;
	}

	if (averaged)
	{
		double totalLastFrameDurations = 0.0;
		//we could save the vals instead of recomputing it, but ... what the hell I have to go on! no optimazations a this non-important level!
		BOOST_FOREACH(double frameDuration, mLastFrameDurations)
		{
			totalLastFrameDurations += frameDuration;
		}

//		for(unsigned int runner= 0 ; runner< mLastFrameDurations.size(); runner ++)
//		{
//			totalLastFrameDurations += mLastFrameDurations[runner];
//		}

		return static_cast<double> (mLastFrameDurations.size())	/ totalLastFrameDurations;
	}
	else
	{
		return 1.0 / mLastFrameDurations.back();
	}
}

double FPSCounter::getLastFrameDuration()const
{
	return mLastFrameDurations.back();
}

int FPSCounter::getTotalRenderedFrames()const
{
	return mNumTotalFrames;
}

}

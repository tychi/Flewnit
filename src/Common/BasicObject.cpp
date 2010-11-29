/**
 *
 * Class BasicObject.cpp
 *
 * This is kind of a debug class for tracking memory leaks and object management; In Release configurations, it should have
 * no functionality.
 *
 * Why i don't use QObject and qt's meta-stuff for this? I don't want qt to penetrate too much of my code, and i fear a performance and memory
 * impact. So I'll implement the neccessary stuff for myself.
 *
 */


#include "BasicObject.h"
#include "Util/Log/Log.h"


#if FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING
#	include "Common/Profiler.h"
#endif


#include <iostream>


#define FLEWNIT_UNSPECIFIED_NAME "none_specified"

namespace Flewnit
{

#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)

	BasicObject::BasicObject()
	:	mUniqueID(FLEWNIT_INVALID_ID)
	{
		registerToProfiler();
	}

	BasicObject::~BasicObject()
	{
		unregisterFromProfiler();
	}


	void BasicObject::registerToProfiler()
	{
		Profiler::getInstancePtr()->registerBasicObject(this);
		//Profiler::getInstancePtr()->printMemoryStatus();
	}

	void BasicObject::unregisterFromProfiler()
	{
		Profiler::getInstancePtr()->unregisterBasicObject(this);
		//Profiler::getInstancePtr()->printMemoryStatus();
	}

	BasicObject* BasicObjectInstancer::getLastRegisteredBasicObjectFromProfiler()
	{
		return Profiler::getInstancePtr()->mRegisteredBasicObjects
				[ Profiler::getInstancePtr()-> mIDOfLastRegisteredButNotMemoryTrackedObject ];
	}

	void BasicObjectInstancer::propagateObjectMemoryFootPrintToProfiler()
	{
		Profiler::getInstancePtr()->registerObjectMemoryFootPrint(
				getLastRegisteredBasicObjectFromProfiler());
	}


#endif

}

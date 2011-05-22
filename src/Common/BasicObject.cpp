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
	:	mUniqueID(FLEWNIT_INVALID_ID),mMemoryFootPrint(0)
	{
		registerToProfiler();
	}

	BasicObject::~BasicObject()
	{
		unregisterFromProfiler();
	}

	String BasicObject::getClassName()const
	{
		//assert( "call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n"
		//		&& (mMemoryFootPrint >0) &&  (mClassName!="")  );

		if((mMemoryFootPrint >0) &&  (mClassName!=""))
		{
			return mClassName;
		}
		else
		{
			LOG<<WARNING_LOG_LEVEL<<"BasicObject::getMemoryFootprint(): Object meta info not initialized yet; call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n";
			return "__INVALID_CLASS_NAME__";
		}
	}


	int BasicObject::getMemoryFootprint()
	{
		//assert( "call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n"
		//		&& (mMemoryFootPrint >0) &&  (mClassName!="")  );

		if((mMemoryFootPrint > 0) &&  (mClassName != ""))
		{
			return mMemoryFootPrint;
		}
		else
		{
			LOG<<WARNING_LOG_LEVEL<<"BasicObject::getMemoryFootprint(): Object meta info not initialized yet; call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n";
			return 0;
		}
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




#endif

}

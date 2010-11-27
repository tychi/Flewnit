/**
 *
 * Class BasicObject.h
 *
 * This is kind of a debug class for tracking memory leaks and object management; In Release configurations, it should have
 * no functionality.
 *
 * Why i don't use QObject and qt's meta-stuff for this? I don't want qt to penetrate too much of my code, and i fear a performance and memory
 * impact. So I'll implement the neccessary stuff for myself.
 *
 */


#pragma once

#include "Common/FlewnitSharedDefinitions.h"

#include <iostream>
#include <cstring>

#define FLEWNIT_UNSPECIFIED_NAME "none_specified"
#define FLEWNIT_STRINGIFY(word) String(#word)

namespace Flewnit
{
	class BasicObject
	{
	public:
		//register to Profiler
#if !(FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING)

		BasicObject(){}
		~BasicObject(){}


//	define the stuff to defaul sonctructor in order to save overhead
#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) BasicObject()
//	define to nothing
#	define SET_MEMORY_FOOTPRINT(className)

#else
		~BasicObject();

//	we need the basic object for tracking purposes
#	define SET_MEMORY_FOOTPRINT(className) mMemoryFootPrint=sizeof(className)

#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) \
		BasicObject(sizeof(className),FLEWNIT_STRINGIFY(className), objectname, purposeDescription)




		BasicObject(
				int memoryFootPrint = -1,
				String className = FLEWNIT_UNSPECIFIED_NAME,
				String objectname = FLEWNIT_UNSPECIFIED_NAME,
				String purposeDescription = FLEWNIT_UNSPECIFIED_NAME);



		void registerToProfiler();
		void unregisterFromProfiler();

	protected:
		int mMemoryFootPrint;


	private:
		ID mUniqueID;

		String mClassName;
		String mObjectName;
		String mPurposeDescription;

#endif
	};

}

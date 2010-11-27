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

#define FLEWNIT_UNSPECIFIED_NAME "none_specified"

namespace Flewnit
{

	class BasicObject
	{
	public:
		//register to Profiler
#if !(FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING)

		BasicObject(){}
		~BasicObject(){}

#else
		BasicObject();
		~BasicObject();

		BasicObject(
				String className = FLEWNIT_UNSPECIFIED_NAME,
				String objectname = FLEWNIT_UNSPECIFIED_NAME,
				String purposeDescription = FLEWNIT_UNSPECIFIED_NAME);


		void registerToProfiler();
		void unregisterFromProfiler();


	private:
		ID mUniqueID;

		String mClassName;
		String mObjectName;
		String mPurposeDescription;
#endif
	};

}

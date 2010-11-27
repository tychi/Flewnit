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

#include <iostream>

#if FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING
# include ""
#endif




#define FLEWNIT_UNSPECIFIED_NAME "none_specified"

namespace Flewnit
{

	BasicObject::BasicObject()
	{
#if FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING

#endif
	}

	BasicObject::BasicObject(String className, String objectname, String purposeDescription)
#if FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING
	: mClassName(className), mObjectName(objectname), mPurposeDescription(purposeDescription)
#endif
	{
		BasicObject();
	}


	BasicObject::~BasicObject()
	{
#if FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING

#endif
	}



}

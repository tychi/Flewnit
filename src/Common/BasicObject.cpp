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

	bool BasicObjectInstancer::mInitializerMacroIsUsed= false;

	BasicObject::BasicObject()
	:	mUniqueID(-1)
	{
		//call the virtual function declared in any derinvnig class by the FLEWNIT_DECLARE_CLASS_META_INFO macro:
		//initMetaInfo();
		assert("declare your meta-info init via the FLEWNIT_INSTANCIATE macro for every class derived from BasicObject!"
				&&  BasicObjectInstancer::getInitializerGuard());
	}


//	BasicObject::BasicObject(int memoryFootPrint, String className, String objectname, String purposeDescription)
//		: mUniqueID(-1), mMemoryFootPrint(memoryFootPrint), mClassName(className), mObjectName(objectname), mPurposeDescription(purposeDescription)
//	{
//		Log::getInstance()<<MEMORY_TRACK_LOG_LEVEL
//				<<"Object of class \""<< mClassName <<"\" created;\n"
//				<<"\t\tMemory footprint:"<< mMemoryFootPrint <<" Byte;\n"
//				<<"\t\tObject name:\"" <<mObjectName <<"\", object purpose: :\""<< mPurposeDescription <<"\";\n" ;
//
//		if(mMemoryFootPrint <=0)
//		{
//			Log::getInstance()<<WARNING_LOG_LEVEL
//				<< "no valid memory footprint was provided by the super class constructor for object \""<<mObjectName<<"\";";
//		}
//
//		registerToProfiler();
//	}

	BasicObject::~BasicObject()
	{
		BasicObjectInstancer::unregisterFromProfiler(this);
	}


//	void BasicObject::registerToProfiler()
//	{
//		//TODO
//	}
//
//	void BasicObject::unregisterFromProfiler()
//	{
//		//TODO
//	}
//

	//-------------------------------------------------------
	void BasicObjectInstancer::registerToProfiler(BasicObject* bo)
	{
		Profiler::getInstancePtr()->registerBasicObject(bo);
		Profiler::getInstancePtr()->printMemoryStatus();
	}

	void BasicObjectInstancer::unregisterFromProfiler(BasicObject* bo)
	{
		Profiler::getInstancePtr()->unregisterBasicObject(bo);
		Profiler::getInstancePtr()->printMemoryStatus();
	}

#endif

}

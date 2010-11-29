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

#include <assert.h>

#include <iostream>
#include <cstring>

namespace Flewnit
{
	class BasicObject
	{
	public:

//---------------------------------------------------------------------------------------------------------
#if !(FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
//without profiling preprocessor option, this is only an empty base class
		BasicObject(){}
		~BasicObject(){}

//#	define FLEWNIT_INSTANTIATE(actualInstantiationExpression, className, purposeDescription) \
//		className * macroTempInstancPtr =  actualInstantiationExpression

#	define FLEWNIT_INSTANTIATE(className,parameterListExpression, purposeDescription) \
		new className parameterListExpression ;


//---------------------------------------------------------------------------------------------------------
#else
//if there is some profiling option, the world gets a bit more complicated:

		//friend Profiler so that he can set the ID of the BasicObjects;
		friend class Profiler;

		friend class BasicObjectInstancer;


		BasicObject();
		~BasicObject();



//#	define FLEWNIT_INSTANTIATE(actualInstantiationExpression, className, purposeDescription) \
//		Flewnit::BasicObjectInstancer::setInitializerGuard(true); \
//		className * macroTempInstancPtr =  actualInstantiationExpression; \
//		Flewnit::BasicObjectInstancer::setMemoryFootPrint((int)sizeof(className),macroTempInstancPtr);\
//		Flewnit::BasicObjectInstancer::setClassName(FLEWNIT_STRINGIFY(className),macroTempInstancPtr);\
//		Flewnit::BasicObjectInstancer::setPurposeDescription(purposeDescription,macroTempInstancPtr);\
//		Flewnit::BasicObjectInstancer::registerToProfiler(macroTempInstancPtr); \
//		Flewnit::BasicObjectInstancer::setInitializerGuard(false)

///\note  I'm doing some crazy haxx here in order to both
///	-assure usage of this macro instead of direct constructor calling by flag setting and
///	-enable support for using this macro like a instatiator function returning a pointer to the instance;
#	define FLEWNIT_INSTANTIATE(className,parameterListExpression, purposeDescription) \
		new className parameterListExpression ; \
		Flewnit::BasicObjectInstancer::setMemoryFootPrint((int)sizeof(className));\
		Flewnit::BasicObjectInstancer::setClassName(FLEWNIT_STRINGIFY(className));\
		Flewnit::BasicObjectInstancer::setPurposeDescription(purposeDescription);\
		Flewnit::BasicObjectInstancer::propagateObjectMemoryFootPrintToProfiler();



		inline int getMemoryFootPrint(){return mMemoryFootPrint;}
		inline const String& getClassName()const{return mClassName;}
		inline const String& getPurposeDescription()const{return mPurposeDescription;}

		inline ID getUniqueID()const{return mUniqueID;}

	private:

		void registerToProfiler();
		void unregisterFromProfiler();

		//only Profiler shall be able to set this:
		ID mUniqueID;

		//must be protected as the class-dependent virtual void initMetaInfo() must set this stuff
		int mMemoryFootPrint;

		String mClassName;
		String mPurposeDescription;


#endif
//---------------------------------------------------------------------------------------------------------
	};


//---------------------------------------------------------------------------------------------------------
#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
	class BasicObjectInstancer
	{
	public:

		static inline void setMemoryFootPrint(int memFP)
		{
			getLastRegisteredBasicObjectFromProfiler()->mMemoryFootPrint=memFP ;
		}

		static inline void setClassName(String name)
		{
			getLastRegisteredBasicObjectFromProfiler()->mClassName=name;
		}

		static inline void setPurposeDescription(String desc)
		{
			getLastRegisteredBasicObjectFromProfiler()->mPurposeDescription=desc ;
		}

		static void propagateObjectMemoryFootPrintToProfiler();

		static BasicObject* getLastRegisteredBasicObjectFromProfiler();

	};
#endif

}

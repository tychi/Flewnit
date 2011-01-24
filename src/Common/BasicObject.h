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

#include <typeinfo>

namespace Flewnit
{
	class BasicObject
	{
	public:

//---------------------------------------------------------------------------------------------------------
#if !(FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
//without profiling preprocessor option, this is only an empty base class
		BasicObject(){}
		virtual ~BasicObject(){}


#	define FLEWNIT_INSTANTIATE(instantiationExpression) \
		instantiationExpression

# 	define FLEWNIT_BASIC_OBJECT_DECLARATIONS


//---------------------------------------------------------------------------------------------------------
#else
//if there is some profiling option, the world gets a bit more complicated:

		//friend Profiler so that he can set the ID of the BasicObjects;
		friend class Profiler;

		friend class BasicObjectInstancer;


		BasicObject();
		virtual ~BasicObject();



/* not needed anymore
///\note  I'm doing some crazy haxx here in order to both
///	-assure usage of this macro instead of direct constructor calling by flag setting and
///	-enable support for using this macro like a instatiator function returning a pointer to the instance;
#	define FLEWNIT_INSTANTIATE(instantiationExpression) \
		instantiationExpression ;
		//Flewnit::BasicObjectInstancer::initAndFinalizeRegistrationOfCurrentBasicObject()
*/


///\{
# 	define FLEWNIT_BASIC_OBJECT_DECLARATIONS \
		public:\
			virtual void initBasicObject() \
			{ \
				mMemoryFootPrint = (int) sizeof(*this); \
				mClassName = String(typeid(*this).name()); \
			} \
		private:
///\}


		virtual void initBasicObject()=0;


		inline const String& getClassName()const
		{
			assert( "call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n"
					&& (mMemoryFootPrint >0) &&  (mClassName!="")  );
			return mClassName;
		}

		//inline const String& getPurposeDescription()const{return mPurposeDescription;}

		inline ID getUniqueID()const{return mUniqueID;}
		inline int getMemoryFootprint()
		{
			assert( "call \"Profiler::getInstance().updateMemoryTrackingInfo()\" (from time to time after object creation) and before querying BasicObjectInfo!\n"
					&& (mMemoryFootPrint >0) &&  (mClassName!="")  );
			return mMemoryFootPrint;
		}

	private:

		void registerToProfiler();
		void unregisterFromProfiler();

		//only Profiler shall be able to set this, so the friendship:
		ID mUniqueID;

	protected:
		//must be protected as the class-dependent virtual void initMetaInfo() must set this stuff
		int mMemoryFootPrint;
		String mClassName;
#endif
//---------------------------------------------------------------------------------------------------------
	};


////---------------------------------------------------------------------------------------------------------
//#if (FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
//	class BasicObjectInstancer
//	{
//	public:
//
////		static inline void setMemoryFootPrint()
////		{
////			getLastRegisteredBasicObjectFromProfiler()->mMemoryFootPrint=
////					getLastRegisteredBasicObjectFromProfiler()->getMemoryFootprint();
////		}
//
//
//		static void initAndFinalizeRegistrationOfCurrentBasicObject();
//
//		static BasicObject* getLastRegisteredBasicObjectFromProfiler();
//
//	};
//#endif

}

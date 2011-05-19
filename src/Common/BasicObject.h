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
		#define FLEWNIT_BASIC_OBJECT_DECLARATIONS
//---------------------------------------------------------------------------------------------------------
#else
//if there is some profiling option, the world gets a bit more complicated:

		//friend Profiler so that it can set the ID of the BasicObjects;
		friend class Profiler;
		BasicObject();
		virtual ~BasicObject();
		//paste the FLEWNIT_BASIC_OBJECT_DECLARATIONS macro into every derived class to init the type information;
		//WARNING: thy type information is only available after all constructors of a class Hierarchy have returned
		//and afteher calling Profiler::updateMemoryTrackingInfo() hererafter; This ugly hack is necessary, because
		//the there is no valid leaf-class-type information retrievable from the this-pointer of an object before
		//all base class constructors have returned; In order not so bother the programmer with calling initBasicObject()
		//in EVERY constructor, one has to call Profiler::updateMemoryTrackingInfo() if one is interested in meta-information;
		//I admit that this "meta object system" is quite weird; It was created just out of interest for class sizes etc;
		//For real profiling, tools like Valgrind are much more appropriate; This "meta object system" was rather a
		//didactic playground in order to finally grasp the c++-internals;
		virtual void initBasicObject()=0;
///\{
		#define FLEWNIT_BASIC_OBJECT_DECLARATIONS \
			public:\
				virtual void initBasicObject() \
				{ \
					mMemoryFootPrint = (int) sizeof(*this); \
					mClassName = String(typeid(*this).name()); \
				} \
			private:
///\}

		inline ID getUniqueID()const{return mUniqueID;}
		String getClassName()const;
		int getMemoryFootprint();

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


}

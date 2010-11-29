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

#	define FLEWNIT_INSTANTIATE(actualInstantiationExpression, className, purposeDescription) \
		className * macroTempInstancPtr =  actualInstantiationExpression


//	define the stuff to default constructor in order to save overhead
//#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) BasicObject()
//#	define FLEWNIT_DECLARE_META_OBJECT(className, objectname, purposeDescription)

//---------------------------------------------------------------------------------------------------------
#else
//if there is some profiling option, the world gets a bit more complicated:

		//friend Profiler so that he can set the ID of the BasicObjects;
		friend class Profiler;

		friend class BasicObjectInstancer;


		BasicObject();
		~BasicObject();

///\note Helper macro to be able to define the constructor call to nothing (i.e. default constructor) if profiling isn't enabled;
///		use this in any constructor of a class deriving from BasicObject;
//#	define BASIC_OBJECT_CONSTRUCTOR(className, objectname, purposeDescription) \
//		BasicObject(sizeof(className),FLEWNIT_STRINGIFY(className), objectname, purposeDescription)

		//virtual void initMetaInfo(){assert("declare your meta-info init via the FLEWNIT_DECLARE_META_OBJECT macro for every class!" && 0);}

///\note macro which should stand in any class definition deriving from BasicObject in order to provide the profiler with some information
//#	define FLEWNIT_DECLARE_CLASS_META_INFO(className, purposeDescription) \
//				virtual void initMetaInfo() \
//				{ \
//					mMemoryFootPrint = sizeof(className); \
//					mClassName = FLEWNIT_STRINGIFY(className); \
//					mPurposeDescription = purposeDescription; \
//				}

//#	define FLEWNIT_INSTANCIATE(className, constructorParamList, purposeDescription) \
//		reinterpret_cast<className*>(BasicObjectInstancer::macroTempInstancePtr) = new className##constructorParamList ; \
//		reinterpret_cast<className*>(BasicObjectInstancer::macroTempInstancePtr)->

//instancePointerToAssignObjectTo = new className##constructorParamList; \

#	define FLEWNIT_INSTANTIATE(actualInstantiationExpression, className, purposeDescription) \
		Flewnit::BasicObjectInstancer::setInitializerGuard(true); \
		className * macroTempInstancPtr =  actualInstantiationExpression; \
		Flewnit::BasicObjectInstancer::setMemoryFootPrint((int)sizeof(className),macroTempInstancPtr);\
		Flewnit::BasicObjectInstancer::setClassName(FLEWNIT_STRINGIFY(className),macroTempInstancPtr);\
		Flewnit::BasicObjectInstancer::setPurposeDescription(purposeDescription,macroTempInstancPtr);\
		Flewnit::BasicObjectInstancer::registerToProfiler(macroTempInstancPtr); \
		Flewnit::BasicObjectInstancer::setInitializerGuard(false)

//		BasicObject(
//				int memoryFootPrint = -1,
//				String className = FLEWNIT_UNSPECIFIED_NAME,
//				String objectname = FLEWNIT_UNSPECIFIED_NAME,
//				String purposeDescription = FLEWNIT_UNSPECIFIED_NAME);


		inline int getMemoryFootPrint(){return mMemoryFootPrint;}
		inline const String& getClassName()const{return mClassName;}
		inline const String& getPurposeDescription()const{return mPurposeDescription;}

		inline ID getUniqueID()const{ assert(mUniqueID>=0); return mUniqueID;}

	private:
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
		static inline void setInitializerGuard(bool value)
		{
			mInitializerMacroIsUsed=value;
		}

		static inline bool getInitializerGuard()
		{
			return mInitializerMacroIsUsed;
		}

		static inline void setMemoryFootPrint(int memFP,BasicObject* bo)
		{
			bo->mMemoryFootPrint=memFP ;
		}

		static inline void setClassName(String name,BasicObject* bo)
		{
			bo->mClassName=name;
		}

		static inline void setPurposeDescription(String desc,BasicObject* bo)
		{
			bo->mPurposeDescription=desc ;
		}

		static void registerToProfiler(BasicObject* bo);

		static void unregisterFromProfiler(BasicObject* bo);

	private:

		static bool mInitializerMacroIsUsed;
	};
#endif

}

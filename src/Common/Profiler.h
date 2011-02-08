/*
 * Profiler.h
 *
 *  Created on: Nov 27, 2010
 *      Author: tychi
 *
 *	\brief The profiler for memory Tracking
 */

#pragma once

#include "Common/FlewnitSharedDefinitions.h"

//---------------------------------------------------------------------------------------------------------
#if !(FLEWNIT_TRACK_MEMORY || FLEWNIT_DO_PROFILING)
//trivial case: define some used macros to nothing


//---------------------------------------------------------------------------------------------------------
#else
//the complicated case:
//TODO think about some buffer allocation macros in order to free the programmer from memoizing and repeating tedious profiling code



#include "Common/Singleton.h"

#include "Buffer/BufferSharedDefinitions.h"
#include "Buffer/BufferInterface.h"



namespace Flewnit
{


class Profiler : public Singleton<Profiler>
{
public:
	//declare friend functions in order to enable only to to (un)register stuff;
	friend void BasicObject::registerToProfiler();
	friend void BasicObject::unregisterFromProfiler();


	//as registerBufferAllocation() is protected member of BufferInterface, only Buffers can use this macro;
	//the friendship between profiler and Bufferinterface::registerBufferAllocation() assures that really only Buffers can register to the profiler;
	friend void BufferInterface::registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
	friend void BufferInterface::unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);

	//call this from time to time to assure good tracking; workaround to omit pure virtual function call of a
	//derived class during constructor execution of base class;
	void updateMemoryTrackingInfo();


	Profiler();
	virtual ~Profiler();

	inline uint getRegisteredObjectCount()
	{
		return mRegisteredBasicObjects.size();
	}
	inline uint getTotalRegisteredObjectMemoryFootPrint()
	{
		return mTotalObjectMemoryFootprint;
	}

	inline uint getTotalAllocatedBufferMemory()
	{
		return  mPrivateAllocatedBufferMemories[HOST_CONTEXT_TYPE] +
				mPrivateAllocatedBufferMemories[OPEN_CL_CONTEXT_TYPE] +
				mPrivateAllocatedBufferMemories[OPEN_GL_CONTEXT_TYPE] +
				mCLGLSharedAllocatedBufferMemory;
	}

	inline uint getTotalContextSpecificAllocatedBufferMemory(ContextType contextType, bool privateAmountOnly)
	{
		return
			mPrivateAllocatedBufferMemories[contextType]	+
			//add shared amount if requestd and if contexttype is gl or cl;
			( (!privateAmountOnly) &&  (contextType!=HOST_CONTEXT_TYPE)) ? mCLGLSharedAllocatedBufferMemory : 0;
	}

	inline uint getNumTotalContextSpecificAllocatedBuffers(ContextType contextType, bool privateAmountOnly)
	{
		return
				mNumPrivateAllocatedBuffers[contextType]	+
			//add shared amount if requested and if contexttype is gl or cl;
			( (!privateAmountOnly) &&  (contextType!=HOST_CONTEXT_TYPE)) ? mNumCLGLSharedAllocatedBuffers : 0;
	}

	inline uint getTotalCLGLSharedAllocatedBufferMemory()
	{
		return mCLGLSharedAllocatedBufferMemory;
	}

	void performBasicChecks();

	void printObjectStatus(BasicObject* bo);

	void printMemoryStatus();
	void printRegisteredObjects();
	void printBufferOnlyMemoryStatus();

	void checkError();
private:

	ID registerBasicObject(BasicObject* bo);
	void registerObjectMemoryFootPrint(BasicObject* bo);

	void unregisterBasicObject(BasicObject* bo);

	void registerBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);
	void unregisterBufferAllocation(ContextTypeFlags contextTypeFlags, size_t sizeInByte);

	//internat routine to omit boilerplate
	void modBufferAllocation_internal(ContextTypeFlags contextTypeFlags, int sizeIncrement, int buffIncrement);

	///\{
	///\brief Members to track IDs and freed IDs for re-usage;
	///\detail Why messing around with ID re-usage? Because if an engine runs for a long time with many objects created and deleted, the IDs will overflow some time;
	/// This may leed to strange bugs; It's very unlikely for this engine to happen, but let's design carefully and with scalability in mind from the beginning;
	std::map<ID,BasicObject*> mRegisteredBasicObjects;


	std::vector<BasicObject*> mRegisteredButUntrackedObjects;


	std::stack<ID> mIDsFromFreedObjects;
	ID mMaxAssignedID;
	///\}

	uint mTotalRegisteredObjects;
	uint mTotalObjectMemoryFootprint;


	uint mPrivateAllocatedBufferMemories[__NUM_CONTEXT_TYPES__];
	uint mCLGLSharedAllocatedBufferMemory;

	uint mNumPrivateAllocatedBuffers[__NUM_CONTEXT_TYPES__];
	uint mNumCLGLSharedAllocatedBuffers;

};

}


#endif // (FLEWNIT_TRACK_MEMORY || FLEWNINT_DO_PROFILING)
//---------------------------------------------------------------------------------------------------------

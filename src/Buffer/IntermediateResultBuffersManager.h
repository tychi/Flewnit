/*
 * IntermediateResultBuffersManager.h
 *
 * A Singleton class which shall help to reduce both the memory consumption
 * of OpenCL programs and to get rid of the overhead of messing around with bunches
 * of intermediate buffers (local scane results etc;)
 *
 *
 *  Created on: Apr 26, 2011
 *      Author: tychi
 */

#pragma once


#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "Buffer/BufferSharedDefinitions.h"


namespace Flewnit
{

class IntermediateResultBuffersManager
	: public Singleton<IntermediateResultBuffersManager>,
	  public BasicObject
{
public:
	IntermediateResultBuffersManager();
	virtual ~IntermediateResultBuffersManager();


	//every functional unit (CL program, class etc) calls this request function,
	//passing the minimum sizes of the required intermediate buffers,
	//IN DESCENDING ORDER!!1 (sry std::sort is so annoying this the comparator objects, have no time for sorting now...
	//.. but will at least verify the order ;( ).
	//The manager stores the maxima of the requestet sizes to mBufferByteSizes;
	void requestBufferAllocation(const std::vector<unsigned int>& minimumBufferByteSizes);

	//after the request phase, the engine calls allocBuffers();
	//afterwards, no new allocation can be requested;
	//the buffers are currently allocated for both host and OpenCL, but not OpenGL!
	//If it will turn out that GL context is of any usage, it will be supplemented ;(
	void allocBuffers()throw(BufferException);

	inline bool buffersAreAllocated()const{return mBuffersAreAllocated;}
	inline bool buffersAreAcquired()const{return (mCurrentOwningKernel == 0 );}

	void acquireBuffersFor(CLKernel* kernel)throw(BufferException);
	void releaseBuffersFor(CLKernel* kernel)throw(BufferException);



private:
	std::vector<unsigned int> mBufferByteSizes;
	std::vector<Buffer*> mSharedIntermediateBuffers;

	bool mBuffersAreAllocated;
	bool mBuffersAreAcquired;

	CLKernel* mCurrentOwningKernel;

};

}


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
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	//every functional unit (CL program, class etc) calls this request function,
	//passing the minimum sizes of the required intermediate buffers,
	//IN DESCENDING ORDER!!1 (sry std::sort is so annoying this the comparator objects, have no time for sorting now...
	//.. but will at least verify the order ;( ).
	//The manager stores the maxima of the requestet sizes to mBufferByteSizes;
	void requestBufferAllocation(const std::vector<size_t>& minimumBufferByteSizes)throw(BufferException);

	//call only valid if  buffersAreAllocated();
	Buffer* getBuffer(unsigned int i)const throw(BufferException);

	inline bool buffersAreAllocated()const{return mBuffersAreAllocated;}

	//{
	//I once thought about a way to save the buffers of beeing concurrently used by kernels;
	//but this concept contradicts the paradigm to invoke kernel as easy as possible without
	//to much special sutff;
	//Plus, the event-waiting mechanism used by my kernels also enables mutual exclusion,
	//although you have the drawback that vou have to know the possibly competing buffer accesse
	//(kernel invocations / buffer reads,writes,copies);
	//TODO enhance the BufferInterface class with a cl::event mamber tracking at least the
	//last reads,writes,copies; (tahsts special stuff for a generic usage of this framework,
	//as long i only simulate particles with openCL, this issue is not that urgent; )
	//	inline bool buffersAreAcquired()const{return (mCurrentOwningKernel == 0 );}
	//	void acquireBuffersFor(CLKernel* kernel)throw(BufferException);
	//	void releaseBuffersFor(CLKernel* kernel)throw(BufferException);
	//	Buffer* getBuffer(CLKernel* kernelPtrForValidation, unsigned int i)const throw(BufferException);
	//}

private:
	friend class CLProgramManager;
	//only instanciable, "buffer-allocatable" and deletable by CLProgramManager
	IntermediateResultBuffersManager();
	virtual ~IntermediateResultBuffersManager();

	//after the request phase, before the kernels are generated and with their defaul arguments
	//(if available),the CLProgramManager singleton calls allocBuffers();
	//this way, at least the intermediate buffers can be used as arguments passed to the CLKernel class
	//and never need to be altered; this way, the user of the CLKernel is not bothered with binding
	//intermediate buffers :).
	//afterwards, no new allocation can be requested;
	//the buffers are currently allocated for both host and OpenCL, but not OpenGL!
	//If it will turn out that GL context is of any usage, it will be supplemented ;(
	void allocBuffers()throw(BufferException);



	std::vector<size_t> mBufferByteSizes;
	std::vector<Buffer*> mSharedIntermediateBuffers;

	bool mBuffersAreAllocated;

	//CLKernel* mCurrentOwningKernel;

};

}


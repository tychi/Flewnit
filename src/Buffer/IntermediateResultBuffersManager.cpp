/*
 * IntermediateResultBuffersManager.cpp
 *
 *  Created on: Apr 26, 2011
 *      Author: tychi
 */

#include "IntermediateResultBuffersManager.h"

#include "Simulator/ParallelComputeManager.h"

#include <boost/foreach.hpp>
#include "Util/HelperFunctions.h"
#include "Buffer/Buffer.h"

namespace Flewnit
{

IntermediateResultBuffersManager::IntermediateResultBuffersManager()
: 	mBuffersAreAllocated(false), mCurrentOwningKernel(0)
{
	//do nothing
}

IntermediateResultBuffersManager::~IntermediateResultBuffersManager()
{
	//nothing to do
}

//every functional unit (CL program, class etc) calls this request function,
//passing the minimum sizes of the required intermediate buffers,
//IN DESCENDING ORDER!!1 (sry std::sort is so annoying this the comparator objects, have no time for sorting now...
//.. but will at least verify the order ;( ).
//The manager stores the maxima of the requestet sizes to mBufferByteSizes;
void IntermediateResultBuffersManager::requestBufferAllocation(
		const std::vector<unsigned int>& minimumBufferByteSizes) throw(BufferException)

{
	assert(minimumBufferByteSizes.size() > 0);


	for(unsigned int i = 1; i< minimumBufferByteSizes.size(); i++)
	{
		if(minimumBufferByteSizes[i-1] < minimumBufferByteSizes[i] )
		{
			throw(BufferException("requested buffer sizes not in descending order!"));
		}


	}

	for(unsigned int i = 0; i< minimumBufferByteSizes.size(); i++)
	{
		if(mBufferByteSizes.size() <= i )
		{
			mBufferByteSizes.push_back(minimumBufferByteSizes[i]);
		}
		else
		{
			mBufferByteSizes[i] = std::max(mBufferByteSizes[i], minimumBufferByteSizes[i]);
		}

	}

}

//after the request phase, the engine calls allocBuffers();
//afterwards, no new allocation can be requested;
//the buffers are currently allocated for both host and OpenCL, but not OpenGL!
//If it will turn out that GL context is of any usage, it will be supplemented ;(
void IntermediateResultBuffersManager::allocBuffers()throw(BufferException)
{
	if(mBuffersAreAllocated)
	{
		throw(BufferException("allocBuffers() was already called before!"));
	}

	mBuffersAreAllocated=true;

	if(PARA_COMP_MANAGER->getParallelComputeDeviceInfo().globalCacheLineSizeByte == 0)
	{
		throw(BufferException("Something went terribly wrong with your OpenCL initialization!"));
	}

	for(unsigned int i = 0; i< mBufferByteSizes.size(); i++)
	{
		unsigned int numUintElements =
			//alloc at least the size of a cache line;
			std::max(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().globalCacheLineSizeByte,
				//align to cache line size;
				HelperFunctions::ceilToNextMultiple(
					mBufferByteSizes[i],
					PARA_COMP_MANAGER->getParallelComputeDeviceInfo().globalCacheLineSizeByte
				)
			)
			//as we denote elements and not byte yount, we have to divied by the byte size of uint
			/ sizeof(unsigned int);

		mSharedIntermediateBuffers.push_back(
		  new Buffer(
			BufferInfo(
				String("intermediateResultBuffer")+ HelperFunctions::toString(i),
				ContextTypeFlags(HOST_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
				INTERMEDIATE_RENDERING_SEMANTICS, //unused
				TYPE_UINT32,
				numUintElements,
				BufferElementInfo(true), //no channeled buffers;
				NO_GL_BUFFER_TYPE,
				NO_CONTEXT_TYPE
			),
			true,
			0
		  )
		);
	}
}


void IntermediateResultBuffersManager::acquireBuffersFor(CLKernel* kernel)throw(BufferException)
{
	if(mCurrentOwningKernel)
	{
		throw(BufferException("buffers are not released!"));
	}

	mCurrentOwningKernel= kernel;
}

void IntermediateResultBuffersManager::releaseBuffersFor(CLKernel* kernel)throw(BufferException)
{
	if(mCurrentOwningKernel != kernel)
	{
		throw(BufferException("You have not right to release buffers you do not own ;("));
	}
	mCurrentOwningKernel = 0;
}

Buffer* IntermediateResultBuffersManager::getBuffer(CLKernel* kernelPtrForValidation, unsigned int i)const throw(BufferException)
{
	if(mCurrentOwningKernel != kernelPtrForValidation)
	{
		throw(BufferException("You have not right to acces the intermediate buffers; acquire them before using them;"));
	}

	assert(i < mSharedIntermediateBuffers.size());

	return mSharedIntermediateBuffers[i];
}


}

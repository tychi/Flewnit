/*
 * CLKernelArguments.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: tychi
 */

#include "CLKernelArguments.h"

#include "Buffer/BufferInterface.h"
#include "Simulator/ParallelComputeManager.h"
#include "CLProgram.h"

namespace Flewnit {

CLKernelArgumentBase::CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr)
: 	mArgName(argName), mArgIndex( (cl_GLuint) (FLEWNIT_INVALID_ID) ),
  	mArgSize(argSizeInByte), mArgValuePtr(argValuePtr)
{

}

void CLKernelArgumentBase::passArgToKernel(CLKernel* clKernel)
{
	assert("arg index set" && (((cl_GLuint)(mArgIndex)) != FLEWNIT_INVALID_ID ));

	GUARD(
		clSetKernelArg(
			clKernel->mKernel(),
			mArgIndex,
			mArgSize,
			mArgValuePtr
		)
	);
}

CLBufferKernelArgument::CLBufferKernelArgument(String argName, BufferInterface* buffi)
	: CLKernelArgumentBase( argName, sizeof(cl_mem),
		&(buffi->getComputeBufferHandle()() )  ),
	  mBufferInterface(buffi)
	{}




//--------------------------------------------------------------------

//CLKernelArguments::CLKernelArguments() {
//	// TODO Auto-generated constructor stub
//
//}
//
//CLKernelArguments::~CLKernelArguments() {
//	// TODO Auto-generated destructor stub
//}






}

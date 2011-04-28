/*
 * CLKernelArguments.cpp
 *
 *  Created on: Apr 24, 2011
 *      Author: tychi
 */

#include "CLKernelArguments.h"

#include "Buffer/BufferInterface.h"
#include "Buffer/PingPongBuffer.h"

#include "Simulator/ParallelComputeManager.h"
#include "CLProgram.h"

#include <boost/foreach.hpp>


namespace Flewnit
{

CLKernelWorkLoadParams::CLKernelWorkLoadParams(
			cl_GLuint numTotalWorkItems,
			cl_GLuint numWorkItemsPerWorkGroup //not guaranteed not to be altered by calculateOptimalParameters()
		)
	: mNumTotalWorkItems(numTotalWorkItems), mNumWorkItemsPerWorkGroup(numWorkItemsPerWorkGroup)
{

}

void CLKernelWorkLoadParams::validateAgainst(CLKernel* kernel)const throw(SimulatorException)
{
	//TODO
	//assert(0 && "TODO implement");
}



//------------------------------------------------------


CLKernelArgumentBase::CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr)
: 	mArgName(argName),
  	mArgSize(argSizeInByte), mArgValuePtr(argValuePtr)
{

}

void CLKernelArgumentBase::passArgToKernel(cl_uint argIndex, CLKernel* clKernel)
{
	assert(clKernel);

	GUARD(
		clSetKernelArg(
			clKernel->mKernel(),
			argIndex,
			mArgSize,
			mArgValuePtr
		)
	);
}

CLBufferKernelArgument::CLBufferKernelArgument(
		String argName,
		BufferInterface* buffi,
		bool ifPingPongBufferUseInactiveOne
)
	: CLKernelArgumentBase(
		argName,
		sizeof(cl_mem),
		( ! ifPingPongBufferUseInactiveOne )
		  ? (void*) ( &( buffi->getComputeBufferHandle()() ) )
		  : (void*) ( &( buffi->toPingPongBuffer().getInactiveBuffer()->getComputeBufferHandle()() ) )
	  ),
	  mBufferInterface(buffi),
	  mIfPingPongBufferUseInactiveOne(ifPingPongBufferUseInactiveOne)
	{}

void CLBufferKernelArgument::set(BufferInterface* buffi, bool ifPingPongBufferUseInactiveOne )
{
	assert(buffi && "CLBufferKernelArgument::set; buffer != 0" );
	assert(
		"ifPingPongBufferUseInactiveOne may only be true if buffi is a ping pong buffer! "&&
		( ! ( ifPingPongBufferUseInactiveOne &&  ( ! buffi->isPingPongBuffer() ) ) )
	);

	mBufferInterface = buffi;
	mArgValuePtr =
		( ! ifPingPongBufferUseInactiveOne )
			? (void*) ( &( buffi->getComputeBufferHandle()() ) )
			: (void*) ( &( buffi->toPingPongBuffer().getInactiveBuffer()->getComputeBufferHandle()() ) );
	mIfPingPongBufferUseInactiveOne =  ifPingPongBufferUseInactiveOne;
}

void CLBufferKernelArgument::passArgToKernel(cl_uint argIndex, CLKernel* clKernel)
{
	//setup robustly againgst ping pong buffer toggle
	set(mBufferInterface, mIfPingPongBufferUseInactiveOne);
	CLKernelArgumentBase::passArgToKernel(argIndex,clKernel);
}


//--------------------------------------------------------------------

CLKernelArguments::CLKernelArguments(const std::vector< CLKernelArgumentBase* > & argVec)
{
	BOOST_FOREACH(CLKernelArgumentBase* arg, argVec)
	{
		mArgVec.push_back(arg);
		assert( "no name may occur more than once" && (mArgMap.find(arg->mArgName) == mArgMap.end() ) );
		mArgMap[arg->mArgName] = arg;
	}
}

CLKernelArguments::~CLKernelArguments()
{
	BOOST_FOREACH(CLKernelArgumentBase* arg, mArgVec)
	{
		delete arg;
	}
}



//at least verify the argument count ;( other info related to a kernel argument list
//is not available yet :((
void CLKernelArguments::validateAgainst(CLKernel* kernel)const throw(SimulatorException)
{

	cl_uint numKernelArgs = kernel->mKernel.getInfo<CL_KERNEL_NUM_ARGS>( PARA_COMP_MANAGER->getLastCLErrorPtr());

	if(numKernelArgs != mArgVec.size())
	{
		throw(SimulatorException(kernel->mKernelName+String(" CLKernel::validate(): number of arguments differ!")));
	}
}

void CLKernelArguments::passArgsToKernel(CLKernel* clKernel)
{
	for(unsigned int i = 0; i < mArgVec.size(); i++)
	{
		mArgVec[i]->passArgToKernel(i, clKernel);
	}
}






//throw exception is arg with name doesn't exist, i < mArgVec.size,
//if a bad cast occured;
CLKernelArgumentBase* CLKernelArguments::getArg(unsigned int i) throw(BufferException)
{
	if(i >= mArgVec.size())
	{
		throw(BufferException("CLKernelArguments::getArg(unsigned int i): index out of range;\n"));
	}

	return mArgVec[i];
}

CLKernelArgumentBase* CLKernelArguments::getArg(String argName) throw(BufferException)
{
	if( mArgMap.find(argName) == mArgMap.end() )
	{
		throw(BufferException(
				String("CLKernelArguments::getArg(String argName): element with specified name ")
				+ argName
				+ String(" does not exist;\n")
		));
	}

	return mArgMap[argName];
}

CLBufferKernelArgument* CLKernelArguments::getBufferArg(String argName) throw(BufferException)
{
	CLBufferKernelArgument* casted = dynamic_cast<CLBufferKernelArgument*>(getArg(argName));
	if(!casted)
	{
		throw(BufferException(
				String("CLKernelArguments::getBufferArg(String argName): bad cast!\n")
		));
	}

	return casted;
}

CLBufferKernelArgument* CLKernelArguments::getBufferArg(unsigned int i) throw(BufferException)
{
	CLBufferKernelArgument* casted = dynamic_cast<CLBufferKernelArgument*>(getArg(i));
	if(!casted)
	{
		throw(BufferException(
				String("CLKernelArguments::getBufferArg(unsigned int i): bad cast!\n")
		));
	}

	return casted;
}








}

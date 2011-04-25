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

CLBufferKernelArgument::CLBufferKernelArgument(String argName, BufferInterface* buffi)
	: CLKernelArgumentBase(
		argName, sizeof(cl_mem),
		&( buffi->getComputeBufferHandle()() )
	  ),
	  mBufferInterface(buffi)
	{}

void CLBufferKernelArgument::set(BufferInterface* buffi )
{
	assert(buffi && "CLBufferKernelArgument::set; buffer != 0" );

	mBufferInterface = buffi;
	mArgValuePtr = &( buffi->getComputeBufferHandle()() );
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




//CLKernelArguments::CLKernelArguments() {
//	// TODO Auto-generated constructor stub
//
//}
//
//CLKernelArguments::~CLKernelArguments() {
//	// TODO Auto-generated destructor stub
//}






}

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
#include "Util/Log/Log.h"


namespace Flewnit
{

CLKernelWorkLoadParams::CLKernelWorkLoadParams(
			cl_GLuint numTotalWorkItems,
			cl_GLuint numWorkItemsPerWorkGroup //not guaranteed not to be altered by calculateOptimalParameters()
		)
	: mNumTotalWorkItems(numTotalWorkItems), mNumWorkItemsPerWorkGroup(numWorkItemsPerWorkGroup)
{

}


//there may be some __attribute__((reqd_work_group_size(...)))
//definitions in the kernel; check that this doesn't conflict with the passed values;
//also check common stuff like that the mNumWorkItemsPerWorkGroup is a power of two;
void CLKernelWorkLoadParams::validateAgainst(CLKernel* kernel)const throw(SimulatorException)
{
	if ( (mNumTotalWorkItems % mNumWorkItemsPerWorkGroup) != 0 )
	{
		throw(SimulatorException("total work item count must be a multiple of work group work item count!"));
	}


	size_t maxPossibleWorkGRoupSizeOfThisKernel =
		kernel->mKernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(
			PARA_COMP_MANAGER->getUsedDevice(),
			PARA_COMP_MANAGER->getLastCLErrorPtr()
	);
	LOG<<INFO_LOG_LEVEL<<"Validation of work load params for kernel "<< kernel->mKernelName << ":\n";
	LOG<<INFO_LOG_LEVEL<<"  Specified desired     work group size of kernel: "<<mNumWorkItemsPerWorkGroup<< ";\n";
	LOG<<INFO_LOG_LEVEL<<"  Technical maximum     work group size of kernel: "<<maxPossibleWorkGRoupSizeOfThisKernel<< ";\n";

	cl::size_t<3> compileTimeRequiredWorkGRoupSizes =
			kernel->mKernel.getWorkGroupInfo<CL_KERNEL_COMPILE_WORK_GROUP_SIZE>(
				PARA_COMP_MANAGER->getUsedDevice(),
				PARA_COMP_MANAGER->getLastCLErrorPtr()
		);
	LOG<<INFO_LOG_LEVEL<<"  Compile time specified work group sizes of kernel: "
			<<
			Vector3Dui(
					(uint)(compileTimeRequiredWorkGRoupSizes[0]),
					(uint)(compileTimeRequiredWorkGRoupSizes[1]),
					(uint)(compileTimeRequiredWorkGRoupSizes[2])
			)<< ";\n";

	cl_ulong usedLocalMemByKernel =
		kernel->mKernel.getWorkGroupInfo<CL_KERNEL_LOCAL_MEM_SIZE>(
				PARA_COMP_MANAGER->getUsedDevice(),
				PARA_COMP_MANAGER->getLastCLErrorPtr()
		);
	LOG<<INFO_LOG_LEVEL<<"  Local memory used by this kernel (bytes): "<<usedLocalMemByKernel<< ";\n";

	if(maxPossibleWorkGRoupSizeOfThisKernel < mNumWorkItemsPerWorkGroup)
	{
		throw(SimulatorException("More work items per work group specified than are possible in this kernel!"));
	}

	if(	( compileTimeRequiredWorkGRoupSizes[1] > 1 ) ||
	    ( compileTimeRequiredWorkGRoupSizes[2] > 1 )    )
	{
		throw(SimulatorException("Sorry there are  multi dim work group dimensions provided as compile time "
				"work group sizes, but this framefork currently only supports 1D work loads :(;"));
	}

	if( //indicator the there was some  __ attribute __((reqd_work_group_size(x,1,1))) specified in kernel
		(compileTimeRequiredWorkGRoupSizes[0] != 0 ) &&
		( mNumWorkItemsPerWorkGroup != (unsigned int)(compileTimeRequiredWorkGRoupSizes[0]) )
	)
	{
		throw(SimulatorException("The compile time work grou size does not fit the now provided one!"));
	}

}



//------------------------------------------------------


CLKernelArgumentBase::CLKernelArgumentBase(String argName, size_t argSizeInByte, void* argValuePtr)
: 	mArgName(argName),
  	mArgSize(argSizeInByte), mArgValuePtr(argValuePtr)
{

}

void CLKernelArgumentBase::passArgToKernel(cl_uint argIndex, CLKernel* clKernel)throw(BufferException)
{
	if(! mArgValuePtr)
	{
		throw(BufferException("CLKernelArgumentBase::passArgToKernel: "
				"arg pointer is still NULL! Set it to a valid value;"));
	}

	assert(clKernel);

//	GUARD(
//		clSetKernelArg(
//			clKernel->mKernel(),
//			argIndex,
//			mArgSize,
//			mArgValuePtr
//		)
//	);

	clKernel->mKernel.setArg(argIndex, mArgSize, mArgValuePtr);

//	cl::KernelFunctor kernelFunctor =
//		clKernel->mKernel.bind(
//				PARA_COMP_MANAGER->getCommandQueue(),
//				cl::NDRange(clKernel->mDefaultKernelWorkLoadParams->mNumTotalWorkItems),
//				cl::NDRange(clKernel->mDefaultKernelWorkLoadParams->mNumWorkItemsPerWorkGroup )
//		);


}

CLBufferKernelArgument::CLBufferKernelArgument(
		String argName,
		BufferInterface* buffi,
		bool ifPingPongBufferUseInactiveOne
)
	: CLKernelArgumentBase(
		argName,
		sizeof(cl_mem),
		0 //set to zero, we handle stuff for buffers ourselves
//		( ! ifPingPongBufferUseInactiveOne )
//		  ? (const void*) ( &( buffi->getComputeBufferHandle()() ) )
//		  : (const void*) ( &( buffi->toPingPongBuffer().getInactiveBuffer()->getComputeBufferHandle()() ) )
	  ),
	  mBufferInterface(buffi),
	  mIfPingPongBufferUseInactiveOne(ifPingPongBufferUseInactiveOne),
	  mCurrentCLMemoryHandle(0)
	{}

void CLBufferKernelArgument::set(BufferInterface* buffi, bool ifPingPongBufferUseInactiveOne )
{
	assert(buffi && "CLBufferKernelArgument::set; buffer != 0" );

	assert(
		"ifPingPongBufferUseInactiveOne may only be true if buffi is a ping pong buffer! "&&
		( ! ( ifPingPongBufferUseInactiveOne &&  ( ! buffi->isPingPongBuffer() ) ) )
	);

//	const ComputeBufferHandle& activeCLMemCppRef = buffi->getComputeBufferHandle();
//	const cl_mem& activeCLMemCRef = activeCLMemCppRef();
//	//const cl_mem* activeCLMemCPtr = & activeCLMemCRef;
//
//	//cl_mem* activeCLMemCPtr = & ( activeCLMemCRef );

	mBufferInterface = buffi;
	mIfPingPongBufferUseInactiveOne = ifPingPongBufferUseInactiveOne;

	if(mIfPingPongBufferUseInactiveOne)
	{
		mCurrentCLMemoryHandle = mBufferInterface->toPingPongBuffer().getInactiveBuffer()->getComputeBufferHandle()();
	}
	else
	{
		mCurrentCLMemoryHandle = mBufferInterface->getComputeBufferHandle()();
	}
	mArgValuePtr = & mCurrentCLMemoryHandle;




}

void CLBufferKernelArgument::passArgToKernel(cl_uint argIndex, CLKernel* clKernel)throw(BufferException)
{
	if(! mBufferInterface)
	{
		throw(BufferException("CLBufferKernelArgument::passArgToKernel: "
				"Buffer pointer is still NULL! Set it to a valid value;"));
	}

	//setup robustly againgst ping pong buffer toggle
	set(mBufferInterface, mIfPingPongBufferUseInactiveOne);

	CLKernelArgumentBase::passArgToKernel(argIndex,clKernel);
	//clKernel->mKernel.setArg(argIndex, mArgSize, mArgValuePtr);
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

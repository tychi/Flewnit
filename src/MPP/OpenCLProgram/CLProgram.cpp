/*
 * Shader.cpp
 *
 *  Created on: Feb 2, 2011
 *      Author: tychi
 */

#include "CLProgram.h"

#include "CLKernelArguments.h"

#include "MPP/OpenCLProgram/CLProgramManager.h"

#include "Simulator/ParallelComputeManager.h"


#include <boost/foreach.hpp>

namespace Flewnit
{

//called by virtual void CLProgram::createKernels();
//those routines build the default arguments list;
CLKernel::CLKernel(CLProgram* owningProgram, String kernelName,
		CLKernelWorkLoadParams* defaultKernelWorkLoadParams,
		CLKernelArguments* kernelArguments)
:
	mKernelName(kernelName),
	mDefaultKernelWorkLoadParams(defaultKernelWorkLoadParams),
	mCLKernelArguments(kernelArguments)

{
//TODO

//	GUARD(
//		mKernel = clCreateKernel(
//			owningProgram->mCLProgram(),
//			kernelName.c_str()
//
//		)
//	);
}


CLKernel::~CLKernel()
{

}

void CLKernel::validate()throw(BufferException)
{
	//TODO
	assert(0&&"TODO implement");
}

cl::Event CLKernel::run(const EventVector& EventsToWaitFor) throw(SimulatorException)
{
	//TODO
	assert(0&&"TODO implement");
}

//run() routine for kernels with different work loads
//Calls customKernelWorkLoadParams.passArgsToKernel();
cl::Event CLKernel::run(
	const EventVector& EventsToWaitFor,
	const CLKernelWorkLoadParams& customKernelWorkLoadParams
) throw(SimulatorException)
{
	//TODO
	assert(0&&"TODO implement");
}





//#################################################################################################

CLProgram::CLProgram(
		Path sourceFileName,
		SimulationDomain sd,
		Path codeDirectory,
		Path programCodeSubFolderName
)
: MPP(sourceFileName.string(), sd),
	mCodeDirectory(codeDirectory),
	mProgramCodeSubFolderName(programCodeSubFolderName)
{

	CLProgramManager::getInstance().registerCLProgram(this);
}

CLProgram::~CLProgram()
{
	BOOST_FOREACH( KernelMap::value_type & pair, mKernels)
	{
		delete pair.second;
	}
}



void CLProgram::build()
{
	//TODO
	assert(0&&"TODO implement");
}

CLKernel* CLProgram::getKernel(String name)throw(SimulatorException)
{
	if(mKernels.find(name) == mKernels.end())
	{
		throw(SimulatorException(
				String("CLProgram: ") + getName()+
				String(": Kernel with specified name ")+ name + String(" doesn't exist!") ));
	}

	return mKernels[name];
}


void CLProgram::validate()throw(SimulatorException)
{
	//TODO
	assert(0&&"TODO implement");
}






}

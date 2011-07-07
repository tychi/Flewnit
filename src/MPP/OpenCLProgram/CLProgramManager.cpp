/*
 * CLProgramManager.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "CLProgramManager.h"

#include "Buffer/IntermediateResultBuffersManager.h"

#include <boost/foreach.hpp>

namespace Flewnit
{

CLProgramManager::CLProgramManager( bool  useCacheUsingOpenCLImplementation)

:
	mIntermediateResultBuffersManager(new IntermediateResultBuffersManager()),
	mUseCacheUsingOpenCLImplementation(useCacheUsingOpenCLImplementation)
{
	// TODO Auto-generated constructor stub

}

CLProgramManager::~CLProgramManager()
{
	delete mIntermediateResultBuffersManager;

	//DON't delete CLPrograms, as they are "owned" by SimResManager;
}


void CLProgramManager::registerCLProgram(CLProgram* clProgram)throw(SimulatorException)
{
	//no validation, as only CLProgram::registerToCLPRogramManager() can call this, which in turn is only
	//callable by CLProgram* base class;

	if(mCLPrograms.find(clProgram->getName()) != mCLPrograms.end())
	{
		throw(SimulatorException(
						String("CLProgramManager::registerCLProgram(): CLProgram with specified name ")
						+ clProgram->getName() + String(" already exists!") ));
	}

	mCLPrograms[ clProgram->getName() ] = clProgram;
}

CLProgram* CLProgramManager::getProgram(String name)throw(SimulatorException)
{
	if(mCLPrograms.find(name) == mCLPrograms.end())
	{
		throw(SimulatorException(
				String("CLProgramManager::getProgram(): CLProgram with specified name ")
				+ name + String(" doesn't exist!") ));
	}

	return mCLPrograms[name];
}



void CLProgramManager::buildProgramsAndCreateKernels()
{
	mIntermediateResultBuffersManager->allocBuffers();

	//BOOST_FOREACH(CLProgram* clProgram, mCLPrograms)
	BOOST_FOREACH(CLProgramMap::value_type stringProgPair, mCLPrograms)
	{
		stringProgPair.second->build();
		stringProgPair.second->createKernels();
//		clProgram->build();
//		clProgram->createKernels();
	}
}


}

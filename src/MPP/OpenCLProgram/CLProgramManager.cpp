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

CLProgramManager::CLProgramManager()
:
	mIntermediateResultBuffersManager(new IntermediateResultBuffersManager())
{
	// TODO Auto-generated constructor stub

}

CLProgramManager::~CLProgramManager()
{
	delete mIntermediateResultBuffersManager;

	//DON't delete CLPrograms, as they are "owned" by SimResManager;
}


void CLProgramManager::registerCLProgram(CLProgram* clProgram)
{
	//no validation, as only CLProgram::registerToCLPRogramManager() can call this, which in turn is only
	//callable by CLProgram* base class;

	mCLPrograms.push_back(clProgram);
}


void CLProgramManager::buildProgramsAndCreateKernels()
{
	mIntermediateResultBuffersManager->allocBuffers();

	BOOST_FOREACH(CLProgram* clProgram, mCLPrograms)
	{
		clProgram->build();
		clProgram->createKernels();
	}
}


}

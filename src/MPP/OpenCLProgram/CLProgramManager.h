/*
 * CLProgramManager.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once


#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "CLProgram.h"

namespace Flewnit
{

class CLProgramManager
	:   public Singleton<CLProgramManager>,
		public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	CLProgramManager();
	virtual ~CLProgramManager();


private:
	//friend void CLProgram::registerToCLPRogramManager();
	friend CLProgram::CLProgram(
			Path sourceFileName,
			SimulationDomain sd,
			Path codeDirectory,
			Path programCodeSubFolderName
	);

	void registerCLProgram(CLProgram* clProgram);

	friend class URE;
	void buildProgramsAndCreateKernels();

	//alloc() function called at beginning of buildProgramsAndCreateKernels, so that
	//all buffer kernel arguments are available on kernel creation;
	IntermediateResultBuffersManager* mIntermediateResultBuffersManager;

	std::vector<CLProgram*> mCLPrograms;


};

}


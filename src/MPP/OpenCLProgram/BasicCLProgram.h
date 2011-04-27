/*
 * BasicCLProgram.h
 *
 * Sets up the template params in the common.cl file;
 *
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once

#include "CLProgram.h"


namespace Flewnit
{

class BasicCLProgram
	:public CLProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	BasicCLProgram(
			Path sourceFileName,
			SimulationDomain sd = GENERIC_SIM_DOMAIN,
			Path codeDirectory = Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH ),
			Path programCodeSubFolderName = Path(String(""))
	);

	virtual ~BasicCLProgram();

protected:
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels()=0;
};

}



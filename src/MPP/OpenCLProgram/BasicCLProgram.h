/*
 * BasicCLProgram.h
 *
 * Sets up the template params in the common.cl file;
 *
 * Can be used directly for reorderAttributes.cl program generation, as this program has no further
 * template dependencies;
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
			Path mProgramCodeSubFolderName = Path(String("")),
			Path codeDirectory = Path( FLEWNIT_DEFAULT_OPEN_CL_KERNEL_SOURCES_PATH ));

	virtual ~BasicCLProgram();

protected:
	virtual void setupTemplateContext(TemplateContextMap& contextMap);
};

}



/*
 * UniformGridRelatedProgram.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once

#include "BasicCLProgram.h"


namespace Flewnit
{

class UniformGridRelatedProgram
	:public BasicCLProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~UniformGridRelatedProgram();

protected:

	//Protected Constructors for derived classes
	//Constructor for
	// - updateUniformGrid.cl
	// - splitAndCompactUniformGrid.cl
	UniformGridRelatedProgram(Path sourceFileName, UniformGrid* uniGrid, SimulationDomain sd = GENERIC_SIM_DOMAIN);


	//calls BasicCLProgram::setupTemplateContext() and sets up uniform grid related template params;
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels()=0;

	UniformGrid* mUniGrid;
};



}


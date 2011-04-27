/*
 * UpdateUniformGridProgram.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once

#include "UniformGridRelatedProgram.h"



namespace Flewnit
{

class UpdateUniformGridProgram
	:public UniformGridRelatedProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~UpdateUniformGridProgram();

protected:

	//Protected Constructors for derived classes
	//Constructor for
	// - updateUniformGrid.cl
	// - splitAndCompactUniformGrid.cl
	UpdateUniformGridProgram(UniformGrid* uniGrid);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

};


}

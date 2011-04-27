/*
 * SplitAndCompactUniformGridProgram.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */
#pragma once

#include "UniformGridRelatedProgram.h"



namespace Flewnit
{

class SplitAndCompactUniformGridProgram
	:public UniformGridRelatedProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~SplitAndCompactUniformGridProgram();

protected:

	//Protected Constructors for derived classes
	//Constructor for
	// - updateUniformGrid.cl
	// - splitAndCompactUniformGrid.cl
	SplitAndCompactUniformGridProgram(UniformGrid* uniGrid);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

};

}

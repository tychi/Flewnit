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

	//updateUniformGrid.cl is created by this class
	UpdateUniformGridProgram(UniformGrid* uniGrid);

	virtual ~UpdateUniformGridProgram();

protected:


	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

};


}

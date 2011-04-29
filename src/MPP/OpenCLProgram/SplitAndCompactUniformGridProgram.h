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

	// - splitAndCompactUniformGrid.cl
	SplitAndCompactUniformGridProgram(UniformGrid* uniGrid);

	//to be called after kernel run to grab the total sum of the scan process;
	unsigned int readBackNumGeneratedNonEmptySplijtCells();

protected:

	//calls UniformGridRelatedProgram::setupTemplateContext() and sets up uniform scan related template params
	//(stream compatction uses the work efficient parallel prefix sum resp. scan);
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

};

}

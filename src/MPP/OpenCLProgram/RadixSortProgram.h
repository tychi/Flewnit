/*
 * RadixSortProgram.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once

#include "BasicCLProgram.h"


namespace Flewnit
{

class RadixSortProgram
	: public BasicCLProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	RadixSortProgram(RadixSorter* radixSorter);
	virtual ~RadixSortProgram();

protected:
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

private:
	RadixSorter* mRadixSorter;
};

}


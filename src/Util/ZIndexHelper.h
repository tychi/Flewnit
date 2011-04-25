/*
 * ZIndexHelper.h
 *
 * Provider of the z-index lookup table and the initial z-Index calculation OpenCL Program;
 *
 *  Created on: Apr 24, 2011
 *      Author: tychi
 *
 *
 */

#pragma once


#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"


namespace Flewnit
{

class ZIndexHelper
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS

public:

	ZIndexHelper(unsigned int numCellsPerDimension) throw(SimulatorException);

	virtual ~ZIndexHelper();

private:

	Buffer* mZIndexLookupTable;

	CLProgram* mCLProgram_initial_CalcZIndex;
};

}



/*
 * UpdateUniformGridProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "UpdateUniformGridProgram.h"


#include "Scene/UniformGrid.h"
#include "Util/HelperFunctions.h"
#include "CLKernelArguments.h"
#include "Simulator/ParallelComputeManager.h"




namespace Flewnit
{

UpdateUniformGridProgram::UpdateUniformGridProgram(UniformGrid* uniGrid)
:
	UniformGridRelatedProgram(String("updateUniformGrid.cl"), uniGrid)
{
	//nothing to do
}

UpdateUniformGridProgram::~UpdateUniformGridProgram()
{
	//nothing to do
}


void UpdateUniformGridProgram::createKernels()
{
	mKernels["kernel_updateUniformGrid"] = new CLKernel(
		this,
		"kernel_updateUniformGrid",

		new CLKernelWorkLoadParams(
			//getNumCellsPerDimension() ^3 work items, one per cell
			mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension(),
			//maximum possible work group size, but floored to power of two
			HelperFunctions::floorToNextPowerOfTwo(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
			)
		),

		new CLKernelArguments(
			{
				//init all arguments to null ptr, because the arguments are variable (there are several UniformGridBuffersets,
				//all are argument candidates)
				new CLBufferKernelArgument("gSortedZIndices", 0),
				new CLBufferKernelArgument("gUniGridCells_ElementStartIndex", 0),
				new CLBufferKernelArgument("gUniGridCells_ElementEndIndexPlus1", 0)
			}
		)
	);
}

}

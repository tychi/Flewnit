/*
 * SplitAndCompactUniformGridProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "SplitAndCompactUniformGridProgram.h"


#include "Scene/UniformGrid.h"
#include "Util/HelperFunctions.h"
#include "CLKernelArguments.h"
#include "Simulator/ParallelComputeManager.h"
#include "Buffer/IntermediateResultBuffersManager.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"

//needed fot the compiler to know tha buffer derives from BufferInterface ;(
//CLBufferKernelArgument constructor call fails;
#include "Buffer/Buffer.h"

#include <grantlee/engine.h>


namespace Flewnit
{

SplitAndCompactUniformGridProgram::SplitAndCompactUniformGridProgram(UniformGrid* uniGrid)
:
	UniformGridRelatedProgram(String("splitAndCompactUniformGrid.cl"), uniGrid)
{
	unsigned int numTotalGridCells =
		mUniGrid->getNumCellsPerDimension()* mUniGrid->getNumCellsPerDimension()* mUniGrid->getNumCellsPerDimension();

	//request intermediate buffers from  IntermediateResultBuffersManager:
	CLProgramManager::getInstance().getIntermediateResultBuffersManager()->requestBufferAllocation(
		std::vector<unsigned int>{

			// - Kernel: 	kernel_scan_localPar_globalSeq
			// - arg:	 	gLocallyScannedTabulatedValues
			// - size:		4 bytes per entry * mNumCellsPerDimension^3
			4 * numTotalGridCells,

			// - Kernel: 	kernel_scan_localPar_globalSeq
			// - arg:	 	gSumsOfPartialGlobalScans
			// - size:		hardware dependent:
			//				4 bytes per entry * (mNumCellsPerDimension^3)/ (2* floorToNextPowerOfTwo(numMaxWorkItems))
			//				Explanation: One work group can locally scan 2*workGroupSize elements;
			//							 We know that the work group size is  floorToNextPowerOfTwo(maxWorkGroupSize);
			//							 So divide total element count by this numbe, then you know how many elements you
			//							 need for a global scan of the local scan total sums;
			(4 * numTotalGridCells)
				/
			(
				2 * HelperFunctions::floorToNextPowerOfTwo(
					PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
				)
			),

			// - Kernel: 	kernel_scan_localPar_globalSeq
			// - arg:	 	gPartiallyGloballyScannedTabulatedValues
			// - size:		at least 4 bytes per entry * (ceilToNextPowerOfTwo(numComputeUnits) + 1);
			//				+1 because the kernel finishing the "total scan" writes out the total sum for read back;;
			HelperFunctions::ceilToNextPowerOfTwo(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxComputeUnits
			) +1
		}
	);


	//mNumCellsPerDimension^3  elements
	//Buffer* gLocallyScannedTabulatedValues;

	//hardware dependent: (mNumCellsPerDimension^3)/ (2*numMaxWorkItems_Base2Floored) elements
	//Buffer* gPartiallyGloballyScannedTabulatedValues;
	//at least NUM_BASE2_CEILED_COMPUTE_UNITS + 1  elements;
	//+1 because the kernel finishing the "total scan" may wanna write out the total sum;
	//--> this is the value defining the number of work groups to be launched for SPH physics sim;
	//Buffer* gSumsOfPartialGlobalScans;
}

SplitAndCompactUniformGridProgram::~SplitAndCompactUniformGridProgram()
{
	//nothing to do
}


//calls BasicCLProgram::setupTemplateContext() and sets up uniform grid related template params;
void SplitAndCompactUniformGridProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	UniformGridRelatedProgram::setupTemplateContext(contextMap);


	contextMap.insert(
		"nvidiaComputeCapabilityMajor",
		PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.x
	);

	contextMap.insert(
		"nvidiaComputeCapabilityMinor",
		PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.y
	);

	contextMap.insert(
		"numArraysToScanInParallel",
		1
	);

	contextMap.insert(
		"scanDataType",
		//scan uint arrays
		"uint"
	);


	contextMap.insert(
		"numTotalElementsToScan",
		 mUniGrid->getNumCellsPerDimension()* mUniGrid->getNumCellsPerDimension()* mUniGrid->getNumCellsPerDimension()
	);



}


void SplitAndCompactUniformGridProgram::createKernels()
{
	mKernels["kernel_scan_localPar_globalSeq"] = new CLKernel(
		this,
		"kernel_scan_localPar_globalSeq",

		new CLKernelWorkLoadParams(
			//getNumCellsPerDimension() ^3 work items, one per cell
			mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension(),
			//maximum possible work group size, but floored to power of two;
			//no memory scarcity here as we scan only one array,n ot 64 like in radix sort or 9
			//like in rigid body update; Hence we can use the maximum work group size for maximum scan efficiency
			HelperFunctions::floorToNextPowerOfTwo(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
			)
		),

		new CLKernelArguments(
			{
				//init the two grid cell buffer arguments to null ptr,
				//because the arguments are variable (there are possobly several UniformGridBuffersets
				//(particle, triangles..), all are argument candidates)
				new CLBufferKernelArgument("gUniGridCells_ElementStartIndex", 0),
				new CLBufferKernelArgument("gUniGridCells_ElementEndIndexPlus1", 0),

				//temporary buffers, grab them from IntermediateResultBuffersManager
				new CLBufferKernelArgument("gLocallyScannedTabulatedValues",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(0) //take the biggest buffer
				),
				new CLBufferKernelArgument("gPartiallyGloballyScannedTabulatedValues",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(1) //take the 2nd biggest buffer
				),
				new CLBufferKernelArgument("gSumsOfPartialGlobalScans",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(2) //take the 3rd biggest buffer
				)

			}
		)
	);


	mKernels["kernel_splitAndCompactUniformGrid"] = new CLKernel(
		this,
		"kernel_splitAndCompactUniformGrid",

		new CLKernelWorkLoadParams(
			//getNumCellsPerDimension() ^3 work items, one per cell
			mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension()*mUniGrid->getNumCellsPerDimension(),
			//maximum possible work group size, but floored to power of two;
			//no memory scarcity here as we scan only one array,n ot 64 like in radix sort or 9
			//like in rigid body update; Hence we can use the maximum work group size for maximum scan efficiency
			HelperFunctions::floorToNextPowerOfTwo(
				PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize
			)
		),

		new CLKernelArguments(
			{
				//init the two grid cell buffer arguments to null ptr,
				//because the arguments are variable (there are possobly several UniformGridBuffersets
				//(particle, triangles..), all are argument candidates)
				new CLBufferKernelArgument("gUniGridCells_ElementStartIndex", 0),
				new CLBufferKernelArgument("gUniGridCells_NumElements", 0),

			    //UniformGridBufferSet to be compacted;
			    //In the physics simulation phase, only "total count of simulation work groups" elements of these Buffers will be used;
				new CLBufferKernelArgument("gCompactedUniGridCells_ElementStartIndex", 0),
				new CLBufferKernelArgument("gCompactedUniGridCells_NumElements", 0),


				//temporary buffers, grab them from IntermediateResultBuffersManager
				new CLBufferKernelArgument("gLocallyScannedTabulatedValues",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(0) //take the biggest buffer
				),
				new CLBufferKernelArgument("gPartiallyGloballyScannedTabulatedValues",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(1) //take the 2nd biggest buffer
				),
				new CLBufferKernelArgument("gSumsOfPartialGlobalScans",
					CLProgramManager::getInstance().getIntermediateResultBuffersManager()
						->getBuffer(2) //take the 3rd biggest buffer
				)

			}
		)
	);
}

}

/*
 * RadixSortProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "RadixSortProgram.h"

#include "Util/RadixSorter.h"
#include "CLKernelArguments.h"

#include "Simulator/ParallelComputeManager.h"
#include "MPP/OpenCLProgram/CLProgramManager.h"
#include "Buffer/IntermediateResultBuffersManager.h"
#include "Util/HelperFunctions.h"

#include "Buffer/Buffer.h"

#include <grantlee/engine.h>


namespace Flewnit
{


RadixSortProgram::RadixSortProgram(RadixSorter* radixSorter)
:
	BasicCLProgram( Path(String("radixSort.cl"))),
	mRadixSorter(radixSorter)
{
		//request intermediate buffers from  IntermediateResultBuffersManager:
		CLProgramManager::getInstance().getIntermediateResultBuffersManager()->requestBufferAllocation(
			std::vector<size_t>{

				// - Kernel: 	phase 1 and 3
				// - arg:	 	gLocallyScannedRadixCounters in both phases
				// - size:		self explaining, see below;
				//				(e.g. 64 * 64k elements for 64 radices per pass * 256k elements to sort / 4 elements per radix counter );
				(size_t) ( sizeof(unsigned int)
					* mRadixSorter->mNumRadicesPerPass
					* ( mRadixSorter->mNumElements / mRadixSorter->mNumElementsPerRadixCounter)
				),

				// - Kernel: 	all three phases
				// - arg:	 	gSumsOfLocalRadixCounts			   in phase 1
				//				gSumsOfLocalRadixCountsToBeScanned in phase 2
				//				gScannedSumsOfLocalRadixCounts     in phase 3
				// - size:		self explaining, see below;  (e.g. 64*512)
				(size_t) ( sizeof(unsigned int)
					* mRadixSorter->mNumRadicesPerPass * mRadixSorter->mNumWorkGroups_TabulationAndReorderPhase
				),

				// - Kernel: 	phase 2 and 3
				// - arg:	 	gPartiallyScannedSumsOfGlobalRadixCounts  in both phases
				// - size:		self explaining, see below;  (e.g. 64)
				(size_t) ( sizeof(unsigned int)
					* mRadixSorter->mNumRadicesPerPass
				),

				// - Kernel: 	phase 2 and 3
				// - arg:	 	gSumsOfPartialScansOfSumsOfGlobalRadixCounts  in both phases
				// - size:		mNumComputeUnits_Base2Ceiled (e.g. 2 or 16 or 32) elements used;
				//				but alloc at least 128 bytes (done automatically by IntermediateResultBuffersManager)
				(size_t) ( sizeof(unsigned int)
					* mRadixSorter->mNumComputeUnits_Base2Ceiled
				)
			}
		);
}

RadixSortProgram::~RadixSortProgram()
{
	// nothing to do
}


void RadixSortProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	BasicCLProgram::setupTemplateContext(contextMap);


	contextMap.insert(
		"nvidiaComputeCapabilityMajor",
		PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.x
	);
	contextMap.insert(
		"nvidiaComputeCapabilityMinor",
		PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.y
	);

	//----------------------------------------------------------------------------
	//explicit scan related stuff:

	//default, different value currently only used in updateRigidBodies.cl
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
		mRadixSorter->mNumElements
	);

	//-----------------------------------------------------
	//radix sort related stuff:

	contextMap.insert(
		"numBitsPerKey",
		mRadixSorter->mNumBitsPerKey
	);
	contextMap.insert(
		"numRadicesPerPass",
		mRadixSorter->mNumRadicesPerPass
	);
	contextMap.insert(
		"log2NumRadicesPerPass",
		HelperFunctions::log2ui(mRadixSorter->mNumRadicesPerPass)
	);
	contextMap.insert(
		"numElementsPerRadixCounter",
		mRadixSorter->mNumElementsPerRadixCounter
	);
	contextMap.insert(
		"localMemForRadixCounters",
		mRadixSorter->mLocalMemForRadixCounters
	);

}

//issue the several createKernel() calls with initial argument list etc;
void RadixSortProgram::createKernels()
{

	//----------------------------------------------------------------------------
	//phase 1

	mKernels["kernel_radixSort_tabulate_localScan_Phase"] = new CLKernel(
			this,
			"kernel_radixSort_tabulate_localScan_Phase",

			new CLKernelWorkLoadParams(
				mRadixSorter->mNumElements,
				mRadixSorter->mNumElements / mRadixSorter->mNumWorkGroups_TabulationAndReorderPhase
			),

			new CLKernelArguments(
				{
					//init the key buffer arguments to null ptr,
					//because the argument is variable, i.e. several different key buffers may want to be sorted
					new CLBufferKernelArgument("gKeysToSort", 0),


					//temporary buffers, grab them from IntermediateResultBuffersManager
					new CLBufferKernelArgument("gLocallyScannedRadixCounters",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(0) //take the biggest buffer
					),
					new CLBufferKernelArgument("gSumsOfLocalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(1) //take the 2nd biggest buffer
					),
					//the only non-buffer kernel arg I'm using so far X-D
					//also variable
					new CLValueKernelArgument<unsigned int>("numPass",0)

				}
			)
		);


	//----------------------------------------------------------------------------
	//phase 2

	mKernels["kernel_radixSort_globalScan_Phase"] = new CLKernel(
			this,
			"kernel_radixSort_globalScan_Phase",

			new CLKernelWorkLoadParams(
				//work group size ...
				(mRadixSorter->mNumWorkGroups_TabulationAndReorderPhase / 2)
				//... times number of work groups = total element count
				* mRadixSorter->mNumComputeUnits_Base2Ceiled ,
				//need half of the to-be-scanned-array-size work items to scan it
				mRadixSorter->mNumWorkGroups_TabulationAndReorderPhase / 2
			),

			new CLKernelArguments(
				{

					//temporary buffers, grab them from IntermediateResultBuffersManager

					new CLBufferKernelArgument("gSumsOfLocalRadixCountsToBeScanned",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(1) //take the 2nd biggest buffer
					),
					new CLBufferKernelArgument("gPartiallyScannedSumsOfGlobalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(2) //take the 3rd biggest buffer
					),
					new CLBufferKernelArgument("gSumsOfPartialScansOfSumsOfGlobalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(3) //take the 4th biggest buffer
					),


					//the only non-buffer kernel arg I'm using so far X-D
					//also variable
					new CLValueKernelArgument<unsigned int>("numPass",0)

				}
			)
		);


	//----------------------------------------------------------------------------
	//phase 3

	mKernels["kernel_radixSort_reorder_Phase"] = new CLKernel(
			this,
			"kernel_radixSort_reorder_Phase",

			new CLKernelWorkLoadParams(
				mRadixSorter->mNumElements,
				mRadixSorter->mNumElements / mRadixSorter->mNumWorkGroups_TabulationAndReorderPhase
			),

			new CLKernelArguments(
				{
					//init the key and "value indices" ping pong buffer arguments to null ptr,
					//because the argument is variable, i.e. several different key buffers may want to be sorted
					new CLBufferKernelArgument("gKeysToSort", 0),
					new CLBufferKernelArgument("gReorderedKeys", 0),

					new CLBufferKernelArgument("gOldIndices", 0),
					new CLBufferKernelArgument("gReorderedOldIndices", 0),

					//temporary buffers, grab them from IntermediateResultBuffersManager

					new CLBufferKernelArgument("gLocallyScannedRadixCounters",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(0) //take the biggest buffer
					),

					new CLBufferKernelArgument("gScannedSumsOfLocalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(1) //take the 2nd biggest buffer
					),
					new CLBufferKernelArgument("gPartiallyScannedSumsOfGlobalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(2) //take the 3rd biggest buffer
					),
					new CLBufferKernelArgument("gSumsOfPartialScansOfSumsOfGlobalRadixCounts",
						CLProgramManager::getInstance().getIntermediateResultBuffersManager()
							->getBuffer(3) //take the 4th biggest buffer
					),


					//the only non-buffer kernel arg I'm using so far X-D
					//also variable
					new CLValueKernelArgument<unsigned int>("numPass",0)

				}
			)
		);


}

}

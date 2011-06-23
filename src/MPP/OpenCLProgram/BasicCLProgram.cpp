/*
 * BasicCLProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "BasicCLProgram.h"

#include "Simulator/ParallelComputeManager.h"
#include "Util/HelperFunctions.h"
#include "MPP/MPP.h"


#include <grantlee/engine.h>
#include "MPP/OpenCLProgram/CLProgramManager.h"
//#include <QtCore/QString>
//#include <QtCore/QVariantHash>


namespace Flewnit
{


BasicCLProgram::BasicCLProgram(
		Path sourceFileName,
		SimulationDomain sd,
		Path codeDirectory,
		Path programCodeSubFolderName
)
: CLProgram(sourceFileName, sd, codeDirectory, programCodeSubFolderName)
{
	//nothing to do
}

BasicCLProgram::~BasicCLProgram()
{
	//everything done by base class
}

void BasicCLProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	contextMap.insert(
		"numComputeUnits_Base2Ceiled",
		HelperFunctions::ceilToNextPowerOfTwo(
			PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxComputeUnits)
	);

	contextMap.insert(
		"numMaxWorkItems_Base2Floored",
		HelperFunctions::floorToNextPowerOfTwo(PARA_COMP_MANAGER->getParallelComputeDeviceInfo().maxWorkGroupSize)
	);

	contextMap.insert(
		"useCacheUsingOpenCLImplementation",
		CLProgramManager::getInstance().useCacheUsingOpenCLImplementation()
	);


	//no support for atomics in devices with compute cap <= 1.1 or 1.2 (TODO check for 1.2 devices in manuals)
	contextMap.insert(
		"atomicsSupport",
		(
			PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.x >= 2
			||
			PARA_COMP_MANAGER->getParallelComputeDeviceInfo().nvidiaDeviceComputeCapability.y >= 2
		)
	);


}

}

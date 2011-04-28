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
}

}

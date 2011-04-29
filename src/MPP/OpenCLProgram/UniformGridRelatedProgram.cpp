/*
 * UniformGridRelatedProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "UniformGridRelatedProgram.h"

#include "Scene/UniformGrid.h"
#include "Util/HelperFunctions.h"

#include <grantlee/engine.h>



namespace Flewnit
{
//Protected Constructors for derived classes
//Constructor for
// - updateUniformGrid.cl
// - splitAndCompactUniformGrid.cl
UniformGridRelatedProgram::UniformGridRelatedProgram(Path sourceFileName, UniformGrid* uniGrid, SimulationDomain sd)
:	BasicCLProgram( sourceFileName, sd),
	mUniGrid(uniGrid)
{

}

UniformGridRelatedProgram::~UniformGridRelatedProgram()
{
	//nothing to do
}

//calls BasicCLProgram::setupTemplateContext() and sets up uniform grid related template params;
void UniformGridRelatedProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	BasicCLProgram::setupTemplateContext(contextMap);

	contextMap.insert(
		"numUniGridCellsPerDimension",
		mUniGrid->getNumCellsPerDimension()
	);

	//unneeded
//	contextMap.insert(
//		"log2NumMaxElementsPerSimulationWorkGroup",
//		HelperFunctions::log2ui( mUniGrid->getNumCellsPerDimension() )
//	);


	contextMap.insert(
		"numMaxElementsPerSimulationWorkGroup",
		mUniGrid->getNumMaxElementsPerSimulationWorkGroup()
	);

	contextMap.insert(
		"log2NumMaxElementsPerSimulationWorkGroup",
		HelperFunctions::log2ui( mUniGrid->getNumMaxElementsPerSimulationWorkGroup() )
	);

}


}

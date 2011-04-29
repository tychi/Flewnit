/*
 * UniformGridRelatedProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "UniformGridRelatedProgram.h"

#include <grantlee/engine.h>
#include "Scene/UniformGrid.h"
#include "Util/HelperFunctions.h"

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


#define NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ numUniGridCellsPerDimension }} )
//default: 6
#define LOG2_NUM_UNIGRID_CELLS_PER_DIMENSION ( {{ log2NumUniGridCellsPerDimension }} )

//default: 32; reason:
//  < 32  --> some threads in warp idle
//  > 32  --> double number of simulation work groups, many threads idle in split up cells;
#define NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP ( {{ numMaxParticlesPerSimulationWorkGroup }} )
//default log2(32) = 5;
#define LOG2_NUM_MAX_PARTICLES_PER_SIMULATION_WORK_GROUP ( {{ log2NumMaxParticlesPerSimulationWorkGroup }}
}


}

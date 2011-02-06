/*
 * SimulationPipelineStage.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SimulationPipelineStage.h"

namespace Flewnit
{

SimulationPipelineStage::SimulationPipelineStage( String name, SimulationDomain sd)
:SimulationObject(name,sd)
{
	// TODO Auto-generated constructor stub

}

SimulationPipelineStage::~SimulationPipelineStage()
{
	// TODO Auto-generated destructor stub
}

//acessor to the different buffers, if subsequent stages need them
BufferInterface* SimulationPipelineStage::getRenderingResult(String what)
{
	if(mRenderingResults.find(what) != mRenderingResults.end())
	{
		return 0;
	}

	return mRenderingResults[what];
}

}

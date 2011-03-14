/*
 * SimulationPipelineStage.cpp
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#include "SimulationPipelineStage.h"

namespace Flewnit
{

SimulationPipelineStage::SimulationPipelineStage( String name, SimulationDomain sd,ConfigStructNode* simConfigNode)
:
		SimulationObject(name,sd),
		mSimConfigNode(simConfigNode)
{
	// TODO Auto-generated constructor stub

}

SimulationPipelineStage::~SimulationPipelineStage()
{
	//nothing to do
}

//acessor to the different buffers, if subsequent stages need them
BufferInterface* SimulationPipelineStage::getRenderingResult(BufferSemantics what)
{
	if(mRenderingResults.find(what) == mRenderingResults.end())
	{
		return 0;
	}

	return mRenderingResults[what];
}

}

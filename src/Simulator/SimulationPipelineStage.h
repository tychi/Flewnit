/*
 * SimulationPipelineStage.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "Common/BasicObject.h"

#include  "SimulatorMetaInfos.h"

namespace Flewnit
{

class SimulationPipelineStage
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	SimulationPipelineRequirements* mSimulationPipelineRequirements;
	SimulationPipelineFeatures*  mSimulationPipelineFeatures;

	Map<String,BufferInterface*> mRenderingResults;

public:
	SimulationPipelineStage();
	virtual ~SimulationPipelineStage();


};

}


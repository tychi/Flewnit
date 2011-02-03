/*
 * SimulationPipelineStage.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once


#include "SimulationObject.h"

#include  "SimulatorMetaInfo.h"

namespace Flewnit
{

class SimulationPipelineStage
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	//SimulationPipelineRequirements* mSimulationPipelineRequirements;
	//SimulationPipelineFeatures*  mSimulationPipelineFeatures;

	Map<String,BufferInterface*> mRenderingResults;

public:
	SimulationPipelineStage( String name, SimulationDomain sd);
	virtual ~SimulationPipelineStage();

	virtual void validateStage() throw(SimulatorException) = 0;

	BufferInterface* getRenderingResult(String what);


};

}


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

public:
	SimulationPipelineStage( String name, SimulationDomain sd, ConfigStructNode* simConfigNode);
	virtual ~SimulationPipelineStage();

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	virtual void initStage()throw(SimulatorException) = 0;
	virtual void validateStage()throw(SimulatorException) = 0;



	//acessor to the different buffers, if subsequent stages need them
	//returns NULL if Buffer with specified name does not exist
	BufferInterface* getRenderingResult(BufferSemantics what);

protected:
	ConfigStructNode* mSimConfigNode;

	std::map<BufferSemantics,BufferInterface*> mRenderingResults;

};

}


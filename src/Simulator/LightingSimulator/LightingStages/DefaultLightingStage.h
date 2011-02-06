/*
 * DefaultLightingStage.h
 *
 *  Created on: Feb 5, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"



namespace Flewnit {


class DefaultLightingStage
: public SimulationPipelineStage
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	DefaultLightingStage();
	virtual ~DefaultLightingStage();

	virtual bool stepSimulation() throw(SimulatorException);
	virtual void initStage()throw(SimulatorException);
	virtual void validateStage()throw(SimulatorException);

};

}


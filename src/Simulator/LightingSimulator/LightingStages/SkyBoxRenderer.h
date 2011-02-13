/*
 * SkyBoxRenderer.h
 *
 *  Created on: Feb 13, 2011
 *      Author: tychi
 */

#pragma once


#include "Simulator/LightingSimulator/LightingSimStageBase.h"

namespace Flewnit {



class SkyBoxRenderer
: public LightingSimStageBase
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	SkyBoxRenderer(ConfigStructNode* simConfigNode);
	virtual ~SkyBoxRenderer();

	virtual bool stepSimulation() throw(SimulatorException);
	virtual void initStage()throw(SimulatorException);
	virtual void validateStage()throw(SimulatorException);

};

}

/*
 * SPHFluidMechanicsSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "../SimulatorInterface.h"

namespace Flewnit
{

class SPHFluidMechanicsSimulator: public SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SPHFluidMechanicsSimulator(ConfigStructNode* simConfigNode);
	virtual ~SPHFluidMechanicsSimulator();

	virtual bool stepSimulation() throw(SimulatorException) ;
	//build pipeline according to config;
	virtual bool initPipeLine()throw(SimulatorException) ;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()throw(SimulatorException) ;

private:



};

}


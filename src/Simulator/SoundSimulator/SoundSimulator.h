/*
 * SoundSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulatorInterface.h"

namespace Flewnit
{

class SoundSimulator: public SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SoundSimulator(ConfigStructNode* simConfigNode);
	virtual ~SoundSimulator();

	virtual bool stepSimulation()throw(SimulatorException) ;
	//build pipeline according to config;
	virtual bool initPipeLine()throw(SimulatorException) ;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()throw(SimulatorException) ;

};

}


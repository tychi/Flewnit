/*
 * SimulatorInterface.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "Common/BasicObject.h"

#include "SimulatorForwards.h"

namespace Flewnit
{

class SimulatorInterface
:public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SimulatorInterface();
	virtual ~SimulatorInterface();

	virtual void stepSimulation()=0;
	//build pipeline according to config;
	virtual void initPipeLine()=0;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()=0;

	//casting functions, assert(0) if wrong casted ;(
	MechanicsSimulator* toMechanicsSimulator();
	LightingSimulator* toLightingSimulator();
	Acustic
};

}


/*
 * MechanicsSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "SimulatorInterface.h"

namespace Flewnit
{

class MechanicsSimulator: public Flewnit::SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	MechanicsSimulator();
	virtual ~MechanicsSimulator();
};

}


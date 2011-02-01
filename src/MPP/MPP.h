/*
 * MPP.h
 *
 *  Created on: Jan 27, 2011
 *      Author: tychi
 *
 *  The "Massively Parallel Program" class, an abstract base class for Shaders and OpenCL kernels
 */

#pragma once

#include "Simulator/SimulationObject.h"


namespace Flewnit
{



class MPP
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	MPP(String name, SimulationDomain sd);
	virtual ~MPP();
};

}

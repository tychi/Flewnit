/*
 * MPP.cpp
 *
 *  Created on: Jan 27, 2011
 *      Author: tychi
 */

#include "MPP.h"
#include "Simulator/SimulationResourceManager.h"

namespace Flewnit
{

MPP::MPP(String name, SimulationDomain sd)
: SimulationObject(name,sd)
{
	SimulationResourceManager::getInstance().registerMPP(this);
}

MPP::~MPP()
{
	// TODO Auto-generated destructor stub
}

}

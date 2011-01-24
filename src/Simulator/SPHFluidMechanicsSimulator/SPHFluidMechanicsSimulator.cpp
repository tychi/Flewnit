/*
 * SPHFluidMechanicsSimulator.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SPHFluidMechanicsSimulator.h"

#include "Util/Log/Log.h"

#include <typeinfo>


namespace Flewnit
{

SPHFluidMechanicsSimulator::SPHFluidMechanicsSimulator(ConfigStructNode* simConfigNode)
:SimulatorInterface(MECHANICAL_SIM_DOMAIN,simConfigNode)
{
	// TODO Auto-generated constructor stub

}

SPHFluidMechanicsSimulator::~SPHFluidMechanicsSimulator()
{
	// TODO Auto-generated destructor stub
}

bool SPHFluidMechanicsSimulator::stepSimulation() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	//LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";
	return true;
}


//build pipeline according to config;
bool SPHFluidMechanicsSimulator::initPipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()\n";
	return true;
}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool SPHFluidMechanicsSimulator::validatePipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine()\n";
	return true;
}



}

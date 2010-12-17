/*
 * LightingSimulator.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "LightingSimulator.h"
#include "Util/Log/Log.h"

#include <typeinfo>

namespace Flewnit
{

LightingSimulator::LightingSimulator()
{
	// TODO Auto-generated constructor stub

}

LightingSimulator::~LightingSimulator()
{
	// TODO Auto-generated destructor stub
}


bool LightingSimulator::stepSimulation()  throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";
	return true;
}


//build pipeline according to config;
bool LightingSimulator::initPipeLine()  throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()";
	return true;
}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool LightingSimulator::validatePipeLine()  throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine()";
	return true;
}



}

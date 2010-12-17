/*
 * SoundSimulator.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: tychi
 */

#include "SoundSimulator.h"

#include "Util/Log/Log.h"

#include <typeinfo>

namespace Flewnit
{

SoundSimulator::SoundSimulator()
{
	// TODO Auto-generated constructor stub

}

SoundSimulator::~SoundSimulator()
{
	// TODO Auto-generated destructor stub
}


bool SoundSimulator::stepSimulation() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";
	return true;
}


//build pipeline according to config;
bool SoundSimulator::initPipeLine()throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()";
	return true;
}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool SoundSimulator::validatePipeLine()throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine()";
	return true;
}

}

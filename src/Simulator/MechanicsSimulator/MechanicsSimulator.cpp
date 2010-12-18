/*
 * MechanicsSimulator.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "MechanicsSimulator.h"

#include "Util/Log/Log.h"

#include <typeinfo>


namespace Flewnit
{

MechanicsSimulator::MechanicsSimulator()
{
	// TODO Auto-generated constructor stub

}

MechanicsSimulator::~MechanicsSimulator()
{
	// TODO Auto-generated destructor stub
}

bool MechanicsSimulator::stepSimulation() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	//LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";
	return true;
}


//build pipeline according to config;
bool MechanicsSimulator::initPipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()";
	return true;
}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool MechanicsSimulator::validatePipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine()";
	return true;
}



}

/*
 * MechanicsSimulator.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "MechanicsSimulator.h"

#include "Util/Log/Log.h"

#include <boost/foreach.hpp>

#include <typeinfo>
#include "Simulator/SimulationPipelineStage.h"


namespace Flewnit
{

MechanicsSimulator::MechanicsSimulator(ConfigStructNode* simConfigNode)
:SimulatorInterface(MECHANICAL_SIM_DOMAIN,simConfigNode)
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
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()\n";

	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
	{
		stage->initStage();
	}

	return true;
}


//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
bool MechanicsSimulator::validatePipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  validatePipeLine()\n";

	bool succes= true;
	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
	{
		succes = stage->validateStage();
	}
	return succes;
}



//bool MechanicsSimulator::validateSimulationStepResults()
//{
//	bool succes= true;
//	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
//	{
//		succes = stage->validateSimulationStepResults();
//	}
//	return succes;
//}
//
//bool MechanicsSimulator::profileAndOptimizeSettings()
//{
//	bool succes= true;
//	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
//	{
//		succes = stage->profileAndOptimizeSettings();
//	}
//	return succes;
//}
//
//bool MechanicsSimulator::profilePerformance()
//{
//	bool succes= true;
//	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
//	{
//		succes = stage->profilePerformance();
//	}
//	return succes;
//}





}

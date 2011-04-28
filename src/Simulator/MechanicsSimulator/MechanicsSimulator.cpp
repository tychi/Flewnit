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
#include "Util/Loader/Config.h"
#include "Util/Loader/LoaderHelper.h"
#include "Simulator/MechanicsSimulator/ParticleMechanicsStages/ParticleMechanicsStage.h"
#include "Simulator/ParallelComputeManager.h"


namespace Flewnit
{

MechanicsSimulator::MechanicsSimulator(ConfigStructNode* simConfigNode)
:SimulatorInterface(MECHANICAL_SIM_DOMAIN,simConfigNode)
{
	// everything  done in initPipeLine()
}

MechanicsSimulator::~MechanicsSimulator()
{
	BOOST_FOREACH(SimulationPipelineStage* stage, mSimStages)
	{
		delete stage;
	}
}

bool MechanicsSimulator::stepSimulation() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	//LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  stepSimulation()";

	PARA_COMP_MANAGER->acquireSharedBuffersForCompute();

	for(unsigned int i=0; i< mSimStages.size(); i++)
	{
		mSimStages[i]->stepSimulation();
	}

	PARA_COMP_MANAGER->acquireSharedBuffersForGraphics();


	return true;
}


//build pipeline according to config;
bool MechanicsSimulator::initPipeLine() throw(SimulatorException)
{
	// TODO Auto-generated destructor stub
	LOG<<DEBUG_LOG_LEVEL<< typeid(*this).name() << " :  initPipeLine()\n";

	unsigned int numStages = mSimConfigNode->get("SimulationPipelineStage").size();

	for(unsigned int i =0 ; i < numStages; i++)
	{
		String stageType = ConfigCaster::cast<String>(
				mSimConfigNode->get("SimulationPipelineStage",i).get("Type",0));

		if(stageType  == "ParticleMechanicsStage")
		{
			mSimStages.push_back(new ParticleMechanicsStage( &( mSimConfigNode->get("SimulationPipelineStage",i) ) ) );
		}

	}

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

/*
 * SimulatorInterface.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "SimulationObject.h"

#include "SimulatorForwards.h"

#include <exception>


namespace Flewnit
{

class ConfigStructNode;


class SimulatorInterface
:public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

protected:

	//allow only creation by factory function
	SimulatorInterface(SimulationDomain sd, ConfigStructNode* simConfigNode);

	ConfigStructNode* mSimConfigNode;

	std::vector<SimulationPipelineStage*> mSimStages;

public:

	virtual ~SimulatorInterface();

	//factory function;
	static SimulatorInterface* create(ConfigStructNode* simConfigNode) throw(SimulatorException);

	//resturns NULL if stage does not exist;
	SimulationPipelineStage* getStage(String name)const; //throw(SimulatorException);

	virtual bool stepSimulation() throw(SimulatorException)  =0;
	//build pipeline according to config;
	virtual bool initPipeLine() throw(SimulatorException) =0;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine() throw(SimulatorException) =0;

//	//non-pure virtual, as not every simulator needs this functionality
//	virtual bool validateSimulationStepResults(){return true;}
//	virtual bool profileAndOptimizeSettings(){return true;}
//	virtual bool profilePerformance(){return true;}

	bool isMechanicsSimulator()const;
	bool isLightingSimulator()const;
	bool isSoundSimulator()const;

	//casting functions, throw exception if wrong casted ;(
	MechanicsSimulator* toMechanicsSimulator()throw(SimulatorException) ;
	LightingSimulator* toLightingSimulator()throw(SimulatorException) ;
	SoundSimulator* toSoundSimulator()throw(SimulatorException) ;
};

}


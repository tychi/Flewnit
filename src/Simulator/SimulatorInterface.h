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

class SimulatorException : public std::exception
{
	String mDescription;
 public:
	SimulatorException(String description = "unspecified simulator error") throw()
	: mDescription(description)
	{ }

	virtual ~SimulatorException() throw(){}

	virtual const char* what() const throw()
	{
	    return mDescription.c_str();
	}
};


class SimulatorInterface
:public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

protected:

	//allow only creation by factory function
	SimulatorInterface(SimulationDomain sd, ConfigStructNode* simConfigNode);

	ConfigStructNode* mSimConfigNode;

public:

	virtual ~SimulatorInterface();

	//factory function;
	static SimulatorInterface* create(ConfigStructNode* simConfigNode) throw(SimulatorException);


	virtual bool stepSimulation() throw(SimulatorException)  =0;
	//build pipeline according to config;
	virtual bool initPipeLine() throw(SimulatorException) =0;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine() throw(SimulatorException) =0;

	//casting functions, assert(0) if wrong casted ;(
	MechanicsSimulator* toMechanicsSimulator()throw(SimulatorException) ;
	LightingSimulator* toLightingSimulator()throw(SimulatorException) ;
	SoundSimulator* toSoundSimulator()throw(SimulatorException) ;
};

}


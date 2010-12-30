/*
 * SimulatorInterface.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SimulatorInterface.h"

//must be include for casting functions ;(
#include "MechanicsSimulator/MechanicsSimulator.h"
#include "LightingSimulator/LightingSimulator.h"
#include "SoundSimulator/SoundSimulator.h"

#include "Util/Loader/Config.h"


namespace Flewnit
{

SimulatorInterface::SimulatorInterface(SimulationDomain sd, ConfigStructNode* simConfigNode)
: SimulationObject( sd, simConfigNode->getName()), mSimConfigNode(simConfigNode)
{
	// TODO Auto-generated constructor stub

}

SimulatorInterface::~SimulatorInterface()
{
	// TODO Auto-generated destructor stub
}



//casting functions, assert(0) if wrong casted ;(
MechanicsSimulator* SimulatorInterface::toMechanicsSimulator() throw(SimulatorException)
{
	MechanicsSimulator* casted = dynamic_cast< MechanicsSimulator* > (this);
	if(casted)
	{
		return casted;
	}
	else
	{
		throw(SimulatorException(String("bad cast to MechanicsSimulator* !")));
		//assert("Bad cast of SimulatorInterface!" && 0);
	}
}

LightingSimulator* SimulatorInterface::toLightingSimulator() throw(SimulatorException)
{
	LightingSimulator* casted = dynamic_cast< LightingSimulator* > (this);
	if(casted)
	{
		return casted;
	}
	else
	{
		throw(SimulatorException(String("bad cast to LightingSimulator* !")));
		//assert("Bad cast of SimulatorInterface!" && 0);
	}
}

SoundSimulator*SimulatorInterface:: toSoundSimulator() throw(SimulatorException)
{
	SoundSimulator* casted = dynamic_cast< SoundSimulator* > (this);
	if(casted)
	{
		return casted;
	}
	else
	{
		throw(SimulatorException(String("bad cast to SoundSimulator* !")));
		//assert("Bad cast of SimulatorInterface!" && 0);
	}
}

SimulatorInterface* SimulatorInterface::create(ConfigStructNode* simConfigNode) throw(SimulatorException)
{
	if(simConfigNode->getName() == "SPHFluidMechanicsSimulator")
	{
		return new MechanicsSimulator(simConfigNode);
	}

	if(simConfigNode->getName() == "LightingSimulator")
	{
		return new LightingSimulator(simConfigNode);
	}

	if(simConfigNode->getName() == "SoundSimulator")
	{
		return new SoundSimulator(simConfigNode);
	}

	String errorString ("Unknown Simulator type: ");
	errorString.append(simConfigNode->getName());
	throw (SimulatorException(errorString));

}

}

/*
 * SimulatorInterface.cpp
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#include "SimulatorInterface.h"

//must be include for casting functions ;(
#include "SPHFluidMechanicsSimulator/SPHFluidMechanicsSimulator.h"
#include "LightingSimulator/LightingSimulator.h"
#include "SoundSimulator/SoundSimulator.h"

#include "Util/Loader/Config.h"
#include "../Util/Loader/LoaderHelper.h"


namespace Flewnit
{

SimulatorInterface::SimulatorInterface(SimulationDomain sd, ConfigStructNode* simConfigNode)
: SimulationObject( simConfigNode->getName(),  sd), mSimConfigNode(simConfigNode)
{
	// TODO Auto-generated constructor stub

}

SimulatorInterface::~SimulatorInterface()
{
	// TODO Auto-generated destructor stub
}



//casting functions, assert(0) if wrong casted ;(
SPHFluidMechanicsSimulator* SimulatorInterface::toSPHFluidMechanicsSimulator() throw(SimulatorException)
{
	SPHFluidMechanicsSimulator* casted = dynamic_cast< SPHFluidMechanicsSimulator* > (this);
	if(casted)
	{
		return casted;
	}
	else
	{
		throw(SimulatorException(String("bad cast to SPHFluidMechanicsSimulator* !")));
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
	String simulatorType = ConfigCaster::cast<String>(simConfigNode->get("Type",0));
	if(simulatorType  == "SPHFluidMechanicsSimulator")
	{
		return new SPHFluidMechanicsSimulator(simConfigNode);
	}

	if(simulatorType == "LightingSimulator")
	{
		return new LightingSimulator(simConfigNode);
	}

	if(simulatorType == "SoundSimulator")
	{
		return new SoundSimulator(simConfigNode);
	}

	String errorString ("Unknown Simulator type: ");
	errorString.append(simConfigNode->getName());
	throw (SimulatorException(errorString));

}

}

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


namespace Flewnit
{

SimulatorInterface::SimulatorInterface()
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

}

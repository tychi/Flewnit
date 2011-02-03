/*
 * SimulationObject.h
 *
 *  Created on: Dec 30, 2010
 *      Author: tychi
 *
 *  Basic abstract class for everything being specialized to a certain simulation domain:
 *  SimulatorInterface,SimulationPipelineStage, Material, Geometry ...
 *
 */

#pragma once

#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"

namespace Flewnit
{

class SimulationObject
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	String mName;
	SimulationDomain mSimDomain;


public:

	SimulationObject( String name, SimulationDomain sd);
	virtual ~SimulationObject();


	inline String getName()const{return mName;}
	inline SimulationDomain getSimDomain()const {return mSimDomain;}

};

}

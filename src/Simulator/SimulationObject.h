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

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class SimulationObject
: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	SimulationDomain mSimDomain;
	String mName;


public:

	SimulationObject(SimulationDomain sd, String name);
	virtual ~SimulationObject();


	inline SimulationDomain getSimDomain()const {return mSimDomain;}
	inline String getName()const{return mName;}

};

}

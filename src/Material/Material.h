/*
 * Material.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *  Base class for all Materials of every simulation domain
 */

#pragma once

#include "Simulator/SimulationObject.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class Material
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;


public:
	//TODO IN KIEL INTEGRATE INSTANCING STUFF ETC IN CONCEPT AND IMPLEMENT IT DIRECTLY
	Material(String name, SimulationDomain sd);
	virtual ~Material();


	virtual bool activate()=0;
	virtual bool deactivate()=0;
};

}


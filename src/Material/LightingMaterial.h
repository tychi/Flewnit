/*
 * LightingMaterial.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 */

#pragma once

#include "Material.h"

#include "Simulator/SimulatorMetaInfos.h"

namespace Flewnit
{

class LightingMaterial
: public Material
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

	SimulationDomain mSimDomain;

public:
	LightingMaterial();
	virtual ~LightingMaterial();


	inline SimulationDomain getSimDomain()const {return mSimDomain;}

	virtual bool activate()=0;
	virtual bool deactivate()=0;
};

}

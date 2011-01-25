/*
 * LightingMaterial.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *
 * 	Base class for all Matrerial in the visual domain.
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


public:
	LightingMaterial(String name);
	virtual ~LightingMaterial();


	virtual bool activate()=0;
	virtual bool deactivate()=0;
};

}

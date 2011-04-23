/*
 * SkyDomeMaterial.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once

#include "Material/VisualMaterial.h"


namespace Flewnit
{

class SkyDomeMaterial
: public VisualMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:

	SkyDomeMaterial(String name, Texture2DCube* cubeTex);
	virtual ~SkyDomeMaterial();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
};

}

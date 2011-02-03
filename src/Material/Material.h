/*
 * Material.h
 *
 *  Created on: Dec 21, 2010
 *      Author: tychi
 *
 *  Base class for all Materials of every simulation domain:
 *  MechanicalMaterial, VisualMaterial, SoundMaterial...
 */

#pragma once

#include "Simulator/SimulationObject.h"

#include "Simulator/SimulatorMetaInfo.h"
#include "Buffer/BufferSharedDefinitions.h"

namespace Flewnit
{


class Material
: public SimulationObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	Material(String name, SimulationDomain sd);
	virtual ~Material();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const = 0;

	virtual bool activate(SimulationPipelineStage* currentStage, SubObject* currentUsingSuboject)=0;
	virtual bool deactivate()=0;
};


}


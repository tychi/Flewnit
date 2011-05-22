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
protected:
	Material(String name, SimulationDomain sd);
public:
	virtual ~Material();

	//check for equality in order to check if a material with the desired properties
	//(shader feature set and textures) already exists in the ResourceManager;
	virtual bool operator==(const Material& rhs) const = 0;

	//The material needs the currentStage for validation and to get the current rendering technique,
	//to delegate its behaviour; (example: do nothing if stage = shadowmap generation stage, cause associated
	//shader to bind the associated textures and use itself otherwise ;) )
	//maybe it can be used for other stuff like render target acquisition and several parameters;
	//The VisualMaterial needs the SubObject to backtrack to the associated geometry and the world object to get the transform;
	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException){}
	//undoing stuff, like re-enable depth test etc.
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException){}
};


}


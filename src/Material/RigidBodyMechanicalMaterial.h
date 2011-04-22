/*
 * RigidBodyMechanicalMaterial.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once


#include "Material/Material.h"
#include "Common/AmendedTransform.h"



namespace Flewnit
{

class RigidBodyMechanicalMaterial
	: public MechanicalMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	RigidBodyMechanicalMaterial(String name, float mass, SceneRepresentation* mechanicalSimulationDomain );
	RigidBodyMechanicalMaterial(String name, float mass, float friction, float restitution);

	virtual ~RigidBodyMechanicalMaterial(String name, float mass);

	virtual bool operator==(const Material& rhs) const;

	//in contrast
	virtual void activate(
			SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);
	//undoing stuff, like re-enable depth test etc.
	virtual void deactivate(SimulationPipelineStage* currentStage,
			SubObject* currentUsingSuboject) throw(SimulatorException);

private:
	//assigned on registration at a spcialized SceneRepresentation in order to associate itself
	//with GPU buffer indices etc;
	uint mRigidBodyID;


};


}

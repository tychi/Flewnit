/*
 * ParticleizedRigidBodyMechanicalMaterial.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "MechanicalMaterial.h"




namespace Flewnit
{

class ParticleRigidBodyMechMat
	: public MechanicalMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	ParticleRigidBodyMechMat(
		String name, float totalMass, float density,
		float friction = 0.5f, float restitution=0.5f);

	virtual ~ParticleRigidBodyMechMat();

	virtual bool operator==(const Material& rhs) const;

	virtual void activate(
				SimulationPipelineStage* currentStage,
				SubObject* currentUsingSuboject) throw(SimulatorException);

	virtual void deactivate(SimulationPipelineStage* currentStage,
				SubObject* currentUsingSuboject) throw(SimulatorException);

private:

	float mDensity;

};

}


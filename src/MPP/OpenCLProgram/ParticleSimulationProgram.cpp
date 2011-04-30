/*
 * ParticleSimulationProgram.cpp
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#include "ParticleSimulationProgram.h"

#include <grantlee/engine.h>
#include "Scene/ParticleSceneRepresentation.h"

namespace Flewnit
{

ParticleSimulationProgram::ParticleSimulationProgram(
		ParticleSimulationProgramType type, UniformGrid* uniGrid, ParticleSceneRepresentation* partScene)
:
	UniformGridRelatedProgram(
		type == INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM
			? "_initial_updateForce_integrate_calcZIndex.cl"
			: (
				type == DENSITIY_PARTICLE_SIM_PROGRAM
				? "updateDensity.cl"
				: "updateForce_integrate_calcZIndex.cl"
			),
			uniGrid,
			MECHANICAL_SIM_DOMAIN
	),
	mParticleSceneRepresentation(partScene),
	mType(type)
{

}

ParticleSimulationProgram::~ParticleSimulationProgram()
{
	//nothing to do
}


//calls UniformGridRelatedProgram::setupTemplateContext() and sets up particle simulation related template params;
void ParticleSimulationProgram::setupTemplateContext(TemplateContextMap& contextMap)
{
	UniformGridRelatedProgram::setupTemplateContext(contextMap);

	contextMap.insert(
		"numMaxParticlesPerRigidBody",
		mParticleSceneRepresentation->getNumMaxParticlesPerRigidBody()
	);


	contextMap.insert(
		"numFluidObjects",
		mParticleSceneRepresentation->getNumMaxFluids()
	);
}


void ParticleSimulationProgram::createKernels()
{

}


}

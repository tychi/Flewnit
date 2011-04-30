/*
 * ParticleSimulationProgram.h
 *

 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once


#include "UniformGridRelatedProgram.h"


namespace Flewnit
{

enum ParticleSimulationProgramType
{
	INIT_FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM,
	DENSITIY_PARTICLE_SIM_PROGRAM,
	FORCE_INTEGRATE_ZINDEX_PARTICLE_SIM_PROGRAM
};



class ParticleSimulationProgram
	: public UniformGridRelatedProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~ParticleSimulationProgram();


	// - _initial_updateForce_integrate_calcZIndex.cl
	// - updateDensity.cl
	// - updateForce_integrate_calcZIndex.cl
	// - updateRigidBodies.cl
	ParticleSimulationProgram(ParticleSimulationProgramType type, ParticleMechanicsStage* particleMechStage);

protected:



	//calls UniformGridRelatedProgram::setupTemplateContext() and sets up particle simulation related template params;
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels();

	ParticleMechanicsStage* mParticleMechanicsStage;
	ParticleSimulationProgramType mParticleSimulationProgramType;

};

}


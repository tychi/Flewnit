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

class ParticleSimulationProgram
	: public UniformGridRelatedProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~ParticleSimulationProgram();

protected:

	//Protected Constructor for derived classes:
	// - _initial_updateForce_integrate_calcZIndex.cl
	// - updateDensity.cl
	// - updateForce_integrate_calcZIndex.cl
	// - updateRigidBodies.cl
	ParticleSimulationProgram(Path sourceFileName, UniformGrid* uniGrid, ParticleSceneRepresentation* partScene);


	//calls UniformGridRelatedProgram::setupTemplateContext() and sets up particle simulation related template params;
	virtual void setupTemplateContext(TemplateContextMap& contextMap);

	//issue the several createKernel() calls with initial argument list etc;
	virtual void createKernels()=0;


	ParticleSceneRepresentation* mPartScene;

};

}


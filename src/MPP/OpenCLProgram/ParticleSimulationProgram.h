/*
 * ParticleSimulationProgram.h
 *

 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once


#include "BasicCLProgram.h"


namespace Flewnit
{

class ParticleSimulationProgram
	: public BasicCLProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	//Constructor for
	// - updateUniformGrid.cl
	// - splitAndCompactUniformGrid.cl
	ParticleSimulationProgram(Path sourceFileName, UniformGrid* uniGrid);
	//Constructor for
	// - _initial_updateForce_integrate_calcZIndex.cl
	// - updateDensity.cl
	// - updateForce_integrate_calcZIndex.cl
	// - updateRigidBodies.cl
	ParticleSimulationProgram(Path sourceFileName, UniformGrid* uniGrid, ParticleSceneRepresentation* partScene);

	virtual ~ParticleSimulationProgram();

protected:

	virtual void setupTemplateContext(TemplateContextMap& contextMap);

private:

	UniformGrid* mUniGrid;
	ParticleSceneRepresentation* mPartScene;
};

}


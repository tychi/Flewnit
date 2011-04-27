/*
 * UpdateForce_Integrate_CalcZIndex_Program.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */


#pragma once



#include "ParticleSimulationProgram.h"


namespace Flewnit
{

class UpdateForce_Integrate_CalcZIndex_Program
	: public ParticleSimulationProgram
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~UpdateForce_Integrate_CalcZIndex_Program();

protected:

	//Protected Constructor for derived classes:
	// - _initial_updateForce_integrate_calcZIndex.cl
	// - updateDensity.cl
	// - updateForce_integrate_calcZIndex.cl
	// - updateRigidBodies.cl
	UpdateForce_Integrate_CalcZIndex_Program(bool initialVersion, UniformGrid* uniGrid, ParticleSceneRepresentation* partScene);


	//issue the several createKernel() calls with initial argument list etc;
	//if initialVersion :
	// - kernel_initial_CalcZIndex
	// - kernel_initial_updateForce_integrate_calcZIndex
	//else:
	// - kernel_updateForce_integrate_calcZIndex
	virtual void createKernels();

	bool mIsInitialVersion;

};

}


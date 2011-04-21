/*
 * MechanicsSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "../SimulatorInterface.h"

#include "Common/AmendedTransform.h"

namespace Flewnit
{

//struct SPHFluidSettings
//{
//	int targetMinSimulationStepsPerSecond;
//	int numFluidParticles;
//	int maxNumRigidBodies;
//
//	int numUniformGridVoxelsPerDimension; //must be power of two due to the z-curve-ordering
//
//	Vector4D uniformGridPosition; //centre of the uniform grid cube;
//	float uniformGridVoxelSize; //uniformGridVoxelSize * numUniformGridVoxelsPerDimension
//								//yields the whole uniform grid extends;
//								//due to repeated mapping of positions to z-indices,
//								//technically, the simulation domain is infinitely large;
//								//but performance will drop extremely due tu overpopulated
//								//z-Index-Voxels and many unncecessary particle calculations;
//								//so try to keep all fluid stuff withing the uniform grid extends;
//
//
//
//	float particleRadius;
//	float fluidParticleMass;
//
//	//int maxParticlesPerRigidBody; //<-- would provoke over-or undersampling, provided we have a unique
//									//	  particle radius; performance issues on oversampling and sponge-like
//									//    RB-penetration by fluid of the RB on undersampling could arise;
//									//	  hence, particelization should be only dependent of the RB world extends
//									//	  (AABB, for example)
//									//	  and the global particle radius.
//	//bool particelizeBordersOnly; //<--bad for density computation...
//	float rigidBodyParticleMass;
//};



class MechanicsSimulator: public SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	MechanicsSimulator(ConfigStructNode* simConfigNode);
	virtual ~MechanicsSimulator();

	virtual bool stepSimulation() throw(SimulatorException) ;
	//build pipeline according to config;
	virtual bool initPipeLine()throw(SimulatorException) ;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()throw(SimulatorException) ;

	//const SPHFluidSettings& getSPHFluidSettings()const{return *mSPHFluidSettings;}


	virtual bool validateSimulationStepResults();
	virtual bool profileAndOptimizeSettings();
	virtual bool profilePerformance();

private:

	//SPHFluidSettings* mSPHFluidSettings;






};

}


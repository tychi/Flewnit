/*
 * SPHFluidMechanicsSimulator.h
 *
 *  Created on: Dec 16, 2010
 *      Author: tychi
 */

#pragma once

#include "../SimulatorInterface.h"

struct SPHFluidSettings
{
	int targetMinSimulationStepsPerSecond ;
	int numFluidParticles;
	float particleRadius;
	float fluidParticleMass;

	int particlesPerRigidBody;
	bool particelizeBordersOnly;
	float rigidBodyParticleMass;
};

namespace Flewnit
{

class SPHFluidMechanicsSimulator: public SimulatorInterface
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;
public:
	SPHFluidMechanicsSimulator(ConfigStructNode* simConfigNode);
	virtual ~SPHFluidMechanicsSimulator();

	virtual bool stepSimulation() throw(SimulatorException) ;
	//build pipeline according to config;
	virtual bool initPipeLine()throw(SimulatorException) ;
	//check if pipeline stages are compatible to each other (also to those stages form other simulators (they might have to interact!))
	virtual bool validatePipeLine()throw(SimulatorException) ;

	const SPHFluidSettings& getSPHFluidSettings()const{return *mSPHFluidSettings;}

private:

	SPHFluidSettings* mSPHFluidSettings;






};

}


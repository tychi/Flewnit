/*
 * ParticleFluidMechanics.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "Simulator/SimulationPipelineStage.h"


namespace Flewnit
{

class ParticleFluidMechanicsStage
	: public SimulationPipelineStage
{
public:
	ParticleFluidMechanicsStage(ConfigStructNode* simConfigNode);
	virtual ~ParticleFluidMechanicsStage();

private:

	//There could come to one's mind to manage Scene representations and accell.
	//structures other than the classic scene graph also centrally by the
	//SimulationResourceManager;
	//Pros: - every class can access stuff conveniently
	//Cons: - Scene maintaining code in SimulationResourceManager gets more complex;
	//		- there could be several scene representations and acc. structures
	//		  of same type but in different usage combinations;
	//		  This would become way too complicated and it won't be used anyway in the
	//		  near future;

	ParticleSceneRepresentation* mParticleSceneRepresentation;
	UniformGrid* mUniformGrid;
};

}


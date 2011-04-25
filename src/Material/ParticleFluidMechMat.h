/*
 * ParticleFluidMechMat.h
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#pragma once


#include "MechanicalMaterial.h"

namespace Flewnit
{

class ParticleFluidMechMat
	: public MechanicalMaterial
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	ParticleFluidMechMat(String name,
			float massPerParticle,
			//is always the density for rigid bodies, is rest density for pressure compuation
			//to numerically stablelize the pressure gradient compuation;
			float restDensity,
			//temperature dependent value for pressure computation, (physically plausible only for fluids,
			//but used to calculate pressure for ANY particle during simulation);
			float gasConstant,
			float viscosity
	);
	virtual ~ParticleFluidMechMat();
};

}



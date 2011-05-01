/*
 * ParticleFluidMechMat.cpp
 *
 *  Created on: Apr 23, 2011
 *      Author: tychi
 */

#include "ParticleFluidMechMat.h"

namespace Flewnit
{

ParticleFluidMechMat::ParticleFluidMechMat(String name,
		unsigned int numContainingParticles,
		float massPerParticle,
		//is always the density for rigid bodies, is rest density for pressure compuation
		//to numerically stablelize the pressure gradient compuation;
		float restDensity,
		//temperature dependent value for pressure computation, (physically plausible only for fluids,
		//but used to calculate pressure for ANY particle during simulation);
		float gasConstant,
		float viscosity
)
	:
	MechanicalMaterial(name, numContainingParticles * massPerParticle),
	mNumContainingParticles(numContainingParticles),
	mMassPerParticle(massPerParticle),
	mRestDensity(restDensity),
	mGasConstant(gasConstant),
	mViscosity(viscosity)
{

}


ParticleFluidMechMat::~ParticleFluidMechMat()
{

}

bool ParticleFluidMechMat::operator==(const Material& rhs) const
{
	const ParticleFluidMechMat* castedMat = dynamic_cast<const ParticleFluidMechMat*>(&rhs);

	if(castedMat)
	{
		return
			(mNumContainingParticles == castedMat->mNumContainingParticles) &&
			( (mMassPerParticle - castedMat->mMassPerParticle ) < FLEWNIT_EPSILON) &&
			( (mRestDensity - castedMat->mRestDensity ) < FLEWNIT_EPSILON) &&
			( (mGasConstant - castedMat->mGasConstant ) < FLEWNIT_EPSILON) &&
			( (mViscosity - castedMat->mViscosity ) < FLEWNIT_EPSILON)
			;
	}

	return false;
}


}

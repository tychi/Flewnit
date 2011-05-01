/*
 * ParticleFluid.cpp
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#include "ParticleFluid.h"
#include "WorldObject/SubObject.h"

#include "Material/VisualMaterial.h"
#include "Material/ParticleFluidMechMat.h"
#include "Geometry/VertexBasedGeometry.h"

namespace Flewnit
{

ParticleFluid::ParticleFluid(
	String name,
	unsigned int particleFluidID,
	VisualMaterial* visMat,
	ParticleFluidMechMat* mechMat,
	VertexBasedGeometry* visualAndMechanicalGeometry
)
:
	WorldObject(
			name,
			FLUID_OBJECT ),
	mParticleFluidID(particleFluidID)
{
	addSubObject(
		new SubObject(
			name + String("VisualSubObject"),
			VISUAL_SIM_DOMAIN,
			visualAndMechanicalGeometry,
			visMat
		)
	);

	addSubObject(
		new SubObject(
			name + String("MechanicalSubObject"),
			MECHANICAL_SIM_DOMAIN,
			visualAndMechanicalGeometry,
			mechMat
		)
	);

}

ParticleFluid::~ParticleFluid()
{

}

}

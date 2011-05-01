/*
 * ParticleFluid.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#pragma once


#include "WorldObject/WorldObject.h"




namespace Flewnit
{




class ParticleFluid
	: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:

	virtual ~ParticleFluid();

private:
	friend class ParticleSceneRepresentation;
	ParticleFluid(
		String name,
		ID particleFluidID, //ID to associate to an index in the ParticleSceneRepresentation's
									  //object Buffers
		VisualMaterial* visMat,
		ParticleFluidMechMat* mechMat,
		VertexBasedGeometry* visualAndMechanicalGeometry
	);

	ID mParticleFluidID;
};


}


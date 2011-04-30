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

//struct FluidConstructionInfo
//{
//	unsigned int numParticles; //should be power of two
//
//	CLshare::ObjectGenericFeatures fluidFeatures;
//
//	CLshare::AABB spawnVolume;
//
//	Vector4D color; //transparency in alpha channel
//	float refractivity;
//	float sprayGenerationSpeed;
//};



class ParticleFluid
	: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	ParticleFluid();
	virtual ~ParticleFluid();
};


}


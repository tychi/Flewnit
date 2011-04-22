/*
 * ParticleFluid.h
 *
 *  Created on: Jan 29, 2011
 *      Author: tychi
 */

#pragma once


#include "WorldObject/WorldObject.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE


namespace Flewnit
{

class ParticleFluid
	: public WorldObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	ParticleFluid(ConfigStructNode* simConfigNode);
	virtual ~ParticleFluid();


};


}


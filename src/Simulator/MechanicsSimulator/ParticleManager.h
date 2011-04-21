/*
 * ParticleManager.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "Common/Singleton.h"
#include "Common/BasicObject.h"

#include "Simulator/SimulatorMetaInfo.h"


namespace Flewnit
{

class ParticleManager:
	public Singleton<ParticleManager>,
	public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	ParticleManager();
	virtual ~ParticleManager();

private:

	UniformGrid* mUniGrid;

	BufferInterface* mPositionPiPoBuffer;
	BufferInterface* mZIndicesPiPoBuffer;
	BufferInterface* mObjectInfoPiPoBuffer;

	BufferInterface* mCorrectedVelocitiesPiPoBuffer;
	BufferInterface* mPredictedVelocitiesPiPoBuffer;
	BufferInterface* mDensitiesPiPoBuffer;
};


}


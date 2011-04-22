/*
 * ParticleSceneRepresentation.h
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#pragma once

#include "SceneRepresentation.h"

#include "Simulator/SimulatorMetaInfo.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

namespace Flewnit
{

class ParticleSceneRepresentation
	:public SceneRepresentation
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	//all initialization is done via XML
	ParticleSceneRepresentation();
	virtual ~ParticleSceneRepresentation();


	inline unsigned int getInvalidObjectID()const{return mNumMaxFluids +1; }


private:


	unsigned int mNumTotalParticles;

	unsigned int mNumMaxFluids;
	unsigned int mNumCurrentFluids;

	unsigned int mNumMaxParticleizedRigidBodies;
	unsigned int mNumCurrentParticleizedRigidBodies;

	unsigned int mNumMaxParticlesPerRigidBody;
	float mVoxelSideLengthRepresentedByRigidBodyParticle;

	std::vector<RigidBody*> mParticleizedRigidBodies;
	std::vector<Fluid*> mParticleFluids;

	//{ pure particle buffers; Initialized to invalid particles
		BufferInterface* mPositionsPiPoBuffer; //vec4 ping pong buffer;

		BufferInterface* mZIndicesPiPoBuffer; //uint ping pong buffer
		BufferInterface* mObjectInfoPiPoBuffer; //uint ping pong buffer

		BufferInterface* mCorrectedVelocitiesPiPoBuffer;
		BufferInterface* mPredictedVelocitiesPiPoBuffer;
		BufferInterface* mDensitiesPiPoBuffer;
	//}


	//{ abstracted but particle-represented object meta info
		//structure of four floats buffer;
		//manage as float4 buffer;
		BufferInterface* mObjectGenericFeaturesBuffer;

		//structure of mixed type buffer;
		//manage as uint- buffer,
		//as this is the digital system's "most native" data type;
		BufferInterface* mRigidBodyBuffer;

		//backtracking buffer;
		BufferInterface* mRigidBodyParticleIndexTableBuffer;

	//}



};


}


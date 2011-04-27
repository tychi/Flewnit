/*
 * ParticleAttributeBuffers.h
 *
 *  Created on: Apr 27, 2011
 *      Author: tychi
 */

#pragma once



#include "Common/BasicObject.h"

#include "Simulator/SimulatorForwards.h"


namespace Flewnit
{

//Pure particle buffers; Optionally initialized to invalid particles;
//All buffers have numTotalParticles elements;
//Info: for 256k particles, all particle attribute buffers consume 41* 2^20 Byte = 41 MB;
class ParticleAttributeBuffers
	: public BasicObject
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS
public:
	ParticleAttributeBuffers(unsigned int numTotalParticles,
			unsigned int invalidObjectID,
			bool initToInvalidObjectID = true);
	virtual ~ParticleAttributeBuffers();
private:
	friend class ParticleSceneRepresentation;

	// convenience function to toggle all ping pong particle attribute/index buffers at once
	void toggleBuffers();
	//after all fluids and rigid bodies have been initialized, they have to be uploaded to the
	//cl device;
	void flushBuffers();
	//usually, all particle attribute data remains on the GPU and don't need to be read back;
	//but for debugging purposes during development, we will need the read-back functionality;
	void readBackBuffers();

	//tracking buffer for fluid objects and rigid bodies to find their belonging particles in the
	//recurrently reordered attribute buffers;
	//used during (at least fluid) rendering as index buffer;
	//mNumTotalParticles elements;
	//no ping pong necessary as no read/write or similar hazard can occur;
	Buffer* mParticleIndexTableBuffer;
	/*
	 * note: the following buffers must all be ping pong buffers, because scattered reading
	 * 		 when reordering after sorting would raise hazards otherwise;
	 * */
	//used to associate a particle with its owning fluid or rigid body object
	//and its particle ID within this object;
	PingPongBuffer* mObjectInfoPiPoBuffer; //uint ping pong buffer

	PingPongBuffer* mZIndicesPiPoBuffer; //uint ping pong buffer
	//the "backtracking" values for reoardering of physical attributes,
	//result of RadixSorter::sort()
	PingPongBuffer* mOldIndicesPiPoBuffer; //uint ping pong buffer
	PingPongBuffer* mPositionsPiPoBuffer; //vec4 ping pong buffer;
	PingPongBuffer* mDensitiesPiPoBuffer;
	PingPongBuffer* mCorrectedVelocitiesPiPoBuffer;
	//the following buffers are needed due to the usage of the velocity verlet integration
	//TODO as this considerably increases bandwidth (also of all things in scattered reordering),
	//we maybe should consider a different integration scheme where no force and only one velocity
	//is sufficient;
	//edit: the big bandwidth eater is the SPH neighbor acquisition (about 1.36 GB!),
	//so copying these buffer maybe is not the biggest memory related issue;
	PingPongBuffer* mPredictedVelocitiesPiPoBuffer;
	PingPongBuffer* mLastStepsAccelerationsPiPoBuffer;


};

}


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


	inline Buffer* getParticleIndexTableBuffer()const{return mParticleIndexTableBuffer;}

	inline PingPongBuffer* getOldIndicesPiPoBuffer()const{return mOldIndicesPiPoBuffer;}
	inline PingPongBuffer* getObjectInfoPiPoBuffer()const{return mObjectInfoPiPoBuffer;}
	inline PingPongBuffer* getZIndicesPiPoBuffer()const{return mZIndicesPiPoBuffer;}

	inline PingPongBuffer* getPositionsPiPoBuffer()const{return mPositionsPiPoBuffer;}
	inline PingPongBuffer* getDensitiesPiPoBuffer()const{return mDensitiesPiPoBuffer;}
	inline PingPongBuffer* getCorrectedVelocitiesPiPoBuffer()const{return mCorrectedVelocitiesPiPoBuffer;}
	inline PingPongBuffer* getPredictedVelocitiesPiPoBuffer()const{return mPredictedVelocitiesPiPoBuffer;}
	inline PingPongBuffer* getLastStepsAccelerationsPiPoBuffer()const{return mLastStepsAccelerationsPiPoBuffer;}


	// convenience function to toggle all ping pong particle attribute/index buffers at once
	//void toggleAllAllBuffers();
	//toggle everything but z indices and old indices, as they aren't reordered but wer USED for reordering ;)
	//a baaad name, but better a bad name than always loosing track of what is toggled an what isn't!!!11
	void toggleAllBuffersButZIndicesAndOldIndices();
	//after all fluids and rigid bodies have been initialized, they have to be uploaded to the
	//cl device;
	void flushBuffers();
	//usually, all particle attribute data remains on the GPU and don't need to be read back;
	//but for debugging purposes during development, we will need the read-back functionality;
	void readBackBuffers();

	//read back and write out buffers to disc for analysis;
	void dumpBuffers(
			String dumpName,
			unsigned int frameNumber,
			bool abortAfterDump,
			bool zIndicesOnly = false
	);
	// the dump files are qite big, so enforce max size;
	#define FLEWNIT_MAX_BUFFER_DUMPS 10

private:

	friend class ParticleSceneRepresentation;
	friend class ReorderParticleAttributesProgram; //make friend for accessing for kernel arg list generation


	unsigned int mNumTotalParticles;

	//tracking buffer for fluid objects and rigid bodies to find their belonging particles in the
	//recurrently reordered attribute buffers;
	//used during (at least fluid) rendering as OpenGL index buffer;
	//no ping pong necessary as no read/write or similar hazard can occur;
	Buffer* mParticleIndexTableBuffer;


	/*
	 * note: the following buffers must all be ping pong buffers, because scattered reading
	 * 		 when reordering after sorting would raise hazards otherwise;
	 * */

	PingPongBuffer* mZIndicesPiPoBuffer; //uint ping pong buffer

	//the "backtracking" values for reordering of physical attributes,
	//result of RadixSorter::sort()
	//the ping pong functionality of mOldIndicesPiPoBuffer is only needed
	//because of the several passes during radix sort;
	//After sorting, this buffer is only used for reordering, afterwards, its values are irrelevant
	//and are overwritten with the next radix sort pass;
	PingPongBuffer* mOldIndicesPiPoBuffer; //uint ping pong buffer


	//used to associate a particle with its owning fluid or rigid body object
	//and its particle ID within this object;
	PingPongBuffer* mObjectInfoPiPoBuffer; //uint ping pong buffer


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


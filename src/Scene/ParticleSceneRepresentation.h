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

#include "UniformGrid.h"



namespace Flewnit
{





class ParticleSceneRepresentation
	:public SceneRepresentation
{
	FLEWNIT_BASIC_OBJECT_DECLARATIONS;

public:
	//all initialization is done via XML
	ParticleSceneRepresentation(
		unsigned int numTotalParticles,
		unsigned int numMaxFluids,
		unsigned int numMaxRigidBodies,
		//should be 1/3* unigrid cell size --> 27 particles/cell-->good
		float voxelSideLengthRepresentedByRigidBodyParticle,
		//zero indicates automatic hardware dependent selection of values;
		unsigned int numMaxParticlesPerRigidBody = 0
	) throw(BufferException);

	virtual ~ParticleSceneRepresentation();


	inline unsigned int getInvalidObjectID()const{return mNumMaxFluids +1; }

	//if rb has not a ParticleRigidBodyMechMat, throw exception as this indicates
	//that it is not a particleized RigidBody;
	//throw exception when particle count > mNumMaxParticlesPerRigidBody;
	//multiply the mechanical geometry position buffer with the global transform, copy to appropriate
	//stride in position buffers etc.. pp; also setup  correctedVelocities, objectInfos
	//and the rest meta info buffers
	void registerRigidBody(ParticleRigidBody* rb) throw(SimulatorException);
	void registerFluid(ParticleFluid* fluid) throw(SimulatorException);

	inline ParticleFluid* getFluid(ID fluidId )const
		{ assert(fluidId < mParticleFluids.size()); return mParticleFluids[fluidId];}
	inline ParticleRigidBody* getRigidBody(ID rbId )const
		{ assert(rbId < mParticleRigidBodies.size()); return mParticleRigidBodies[rbId];}

private:

	//precondition: UniformGrid::sort() has already been called for this step,
	//		 		i.e. z-Index sort has already been performed, so that
	//mOldIndicesPiPoBuffer can be used to reorder the attributes;
	void reorderAttributes();

	//take all "pure particle buffers" (except predicted velocities, this buffer is only needed for CL integration ),
	//assign them to the appropriate semantics attribute binding point (POSITION_SEMANTICS etc.),
	//bind mParticleIndexTableBuffer as index buffer with appropriate offset and particle count;
	VertexBasedGeometry* compileAttributeBuffersToGeometryForFluid(ParticleFluid* fluid);

	unsigned int mNumTotalParticles;

	unsigned int mNumMaxFluids;
	unsigned int mNumCurrentFluids;

	unsigned int mNumMaxRigidBodies;
	unsigned int mNumCurrentRigidBodies;

	unsigned int mNumMaxParticlesPerRigidBody;
	float mVoxelSideLengthRepresentedByRigidBodyParticle;



	std::vector<ParticleRigidBody*> mParticleRigidBodies;
	std::vector<ParticleFluid*> mParticleFluids;

	CLProgram* mCLProgram_reorderAttributes;

	//{ abstracted but particle-represented object meta info

		//structure of four floats buffer;
		//mNumMaxFluids +  mNumMaxRigidBodies +1 elements;
		//	(+1 because of the invalid object features; mass=0,density=infinite)
		//manage as float4 buffer;
		Buffer* mObjectGenericFeaturesBuffer;

		//structure of mixed type buffer;
		//manage as uint- buffer,
		//as this is the digital system's "most native" data type;
		//mNumMaxRigidBodies elements;
		Buffer* mRigidBodyBuffer;


		/* following deprecated; TODO delete
			//tracking buffer for rigid bodies to find their belonging particles in the
			//recurrently reordered attribute buffers;
			//mNumMaxParticleizedRigidBodies * mNumMaxParticlesPerRigidBody uint elements
			Buffer* mRigidBodyParticleIndexTableBuffer;
		*/
	//}

	//{
	//Pure particle buffers; Initialized to invalid particles;
	//All buffers have mNumTotalParticles elements;
	//Info: for 256k particles, all particle buffers consume 41* 2^20 Byte = 41 MB;


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
	//}






};


}


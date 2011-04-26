/*
 * ParticleSceneRepresentation
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "ParticleSceneRepresentation.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Simulator/SimulationResourceManager.h"
#include "Buffer/Buffer.h"
#include "MPP/OpenCLProgram/CLProgram.h"
#include "Buffer/PingPongBuffer.h"


namespace Flewnit
{

ParticleAttributeBuffers::ParticleAttributeBuffers(
		unsigned int numTotalParticles,
		unsigned int invalidObjectID,
		bool initToInvalidObjectID)
{
	BufferInfo glCLSharedIndexBufferInfo(
		"particleIndexTableBuffer",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		INDEX_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_INDEX_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	mParticleIndexTableBuffer = new Buffer(	glCLSharedIndexBufferInfo,	true, 0 );


	BufferInfo glCLSharedObjectInfoBufferInfo(
		"particleObjectInfoBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		PRIMITIVE_ID_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	Buffer* ping = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	glCLSharedObjectInfoBufferInfo.name = "particleObjectInfoBufferPong";
	Buffer* pong = new Buffer(	glCLSharedObjectInfoBufferInfo,	true, 0 );
	mObjectInfoPiPoBuffer = new PingPongBuffer("particleObjectInfoPiPoBuffer",ping,pong);


	BufferInfo glCLSharedZIndicesBufferInfo(
		"particleZIndicesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		Z_INDEX_SEMANTICS,
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedZIndicesBufferInfo,	true, 0 );
	glCLSharedZIndicesBufferInfo.name = "particleZIndicesBufferPong";
	pong = new Buffer(	glCLSharedZIndicesBufferInfo,	true, 0 );
	mZIndicesPiPoBuffer = new PingPongBuffer("particleZIndicesPiPoBuffer",ping,pong);

	BufferInfo glCLSharedOldIndicesBufferInfo(
		"particleOldIndicesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		CUSTOM_SEMANTICS, //actually, it is not planned to bind this buffer as vertex  attribute to GL,
						  //hence it has no real semantics, it is just a helper buffer for sorting;
						  //but maybe for debug purposes, one would want to bind it anyway ;(...
		TYPE_UINT32,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_UINT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	glCLSharedOldIndicesBufferInfo.name = "particleOldIndicesBufferPong";
	pong = new Buffer(	glCLSharedOldIndicesBufferInfo,	true, 0 );
	mOldIndicesPiPoBuffer = new PingPongBuffer("particleOldIndicesPiPoBuffer",ping,pong);


	//-------------------------------------------------------------------------------------------------------------------
	//beginning with the actual physical attributes:

	BufferInfo glCLSharedPositionsBufferInfo(
		"particlePositionsBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		POSITION_SEMANTICS,
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedPositionsBufferInfo,	true, 0 );
	glCLSharedPositionsBufferInfo.name = "particlePositionsBufferPong";
	pong = new Buffer(	glCLSharedPositionsBufferInfo,	true, 0 );
	mPositionsPiPoBuffer = new PingPongBuffer("particlePositionsPiPoBuffer",ping,pong);

	BufferInfo glCLSharedDensitiesBufferInfo(
		"particleDensitiesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		DENSITY_SEMANTICS,
		TYPE_FLOAT,
		numTotalParticles,
		BufferElementInfo(1,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedDensitiesBufferInfo,	true, 0 );
	glCLSharedDensitiesBufferInfo.name = "particleDensitiesBufferPong";
	pong = new Buffer(	glCLSharedDensitiesBufferInfo,	true, 0 );
	mDensitiesPiPoBuffer = new PingPongBuffer("particleDensitiesPiPoBuffer",ping,pong);


	BufferInfo glCLSharedVelocitiesBufferInfo(
		"particleCorrectedVelocitiesBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		VELOCITY_SEMANTICS,
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	glCLSharedVelocitiesBufferInfo.name = "particleCorrectedVelocitiesBufferPong";
	pong = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	mCorrectedVelocitiesPiPoBuffer = new PingPongBuffer("particleCorrectedVelocitiesPiPoBuffer",ping,pong);

	//at least one time, a buffer infor object can be reused ;)
	glCLSharedVelocitiesBufferInfo.name = "particlePredictedVelocitiesBufferPing";
	ping = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	glCLSharedVelocitiesBufferInfo.name = "particlePredictedVelocitiesBufferPong";
	pong = new Buffer(	glCLSharedVelocitiesBufferInfo,	true, 0 );
	mPredictedVelocitiesPiPoBuffer = new PingPongBuffer("particlePredictedVelocitiesPiPoBuffer",ping,pong);



	BufferInfo glCLSharedAccelerationsBufferInfo(
		"particleAccelerationsBufferPing",
		//all three contexts;
		ContextTypeFlags(  HOST_CONTEXT_TYPE_FLAG | OPEN_GL_CONTEXT_TYPE_FLAG | OPEN_CL_CONTEXT_TYPE_FLAG),
		FORCE_SEMANTICS, //k, have to refactor ;(.. but F=m*a, particle mass is known ;)
		TYPE_VEC4F,
		numTotalParticles,
		BufferElementInfo(4,GPU_DATA_TYPE_FLOAT,32,false),
		VERTEX_ATTRIBUTE_BUFFER_TYPE,
		NO_CONTEXT_TYPE
	);
	ping = new Buffer(	glCLSharedAccelerationsBufferInfo,	true, 0 );
	glCLSharedAccelerationsBufferInfo.name = "particleAccelerationsBufferPong";
	pong = new Buffer(	glCLSharedAccelerationsBufferInfo,	true, 0 );
	mLastStepsAccelerationsPiPoBuffer = new PingPongBuffer("particleAccelerationsPiPoBuffer",ping,pong);

	//-------------------------------------------------------------------------------------------------------
	if(initToInvalidObjectID)
	{
		//mObjectInfoPiPoBuffer->getCPUBufferHandle()

	}

}

ParticleAttributeBuffers::~ParticleAttributeBuffers()
{
	//nothing to do, buffers are deleted by SimResourceMan.
}

//after all fluids and rigid bodies have been initialized, they have to be uploaded to the
//cl device;
void ParticleAttributeBuffers::flushBuffers()
{
	//what an amazing piece of code this routine is ;)

	mParticleIndexTableBuffer->copyFromHostToGPU();

	mObjectInfoPiPoBuffer->copyFromHostToGPU();
	mZIndicesPiPoBuffer->copyFromHostToGPU();
	mOldIndicesPiPoBuffer->copyFromHostToGPU();
	mPositionsPiPoBuffer->copyFromHostToGPU();
	mDensitiesPiPoBuffer->copyFromHostToGPU();
	mCorrectedVelocitiesPiPoBuffer->copyFromHostToGPU();
	mPredictedVelocitiesPiPoBuffer->copyFromHostToGPU();
	mLastStepsAccelerationsPiPoBuffer->copyFromHostToGPU();

}

//usually, all particle attribute data remains on the GPU and don't need to be read back;
//but for debugging purposes during development, we will need the read-back functionality;
void ParticleAttributeBuffers::readBackBuffers()
{
	//what an amazing piece of code this routine is ;)

	mParticleIndexTableBuffer->readBack();

	mObjectInfoPiPoBuffer->readBack();
	mZIndicesPiPoBuffer->readBack();
	mOldIndicesPiPoBuffer->readBack();
	mPositionsPiPoBuffer->readBack();
	mDensitiesPiPoBuffer->readBack();
	mCorrectedVelocitiesPiPoBuffer->readBack();
	mPredictedVelocitiesPiPoBuffer->readBack();
	mLastStepsAccelerationsPiPoBuffer->readBack();
}




//#######################################################################################################

ParticleSceneRepresentation::ParticleSceneRepresentation(
		unsigned int numTotalParticles,
		unsigned int numMaxFluids,
		unsigned int numMaxRigidBodies,
		//be careful with this value: the maximum may be hardware/implementation dependent and hence may be clamped;
		//GT200: max 256; Fermi: max 1024; reason: scarce local memory,
		//hence value reloading for greater particle counts;
		//not implemented a compromise allowing greater RB particle count for lesser performance yet (april 2011)
		unsigned int numMaxParticlesPerRigidBody,
		//should be 1/3* unigrid cell size --> 27 particles/cell-->good
		float voxelSideLengthRepresentedByRigidBodyParticle
	) throw(BufferException)
	:
	SceneRepresentation(PARTICLE_SCENE_REPRESENTATION),

	mNumTotalParticles(numTotalParticles),
	mNumMaxFluids(numMaxFluids),
	mNumCurrentFluids(0),
	mNumMaxRigidBodies(numMaxRigidBodies),
	mNumCurrentRigidBodies(0),
	mNumMaxParticlesPerRigidBody(numMaxParticlesPerRigidBody),
  	mVoxelSideLengthRepresentedByRigidBodyParticle(voxelSideLengthRepresentedByRigidBodyParticle),

	mObjectGenericFeaturesBuffer(0),
	mRigidBodyBuffer(0),
	mRigidBodyRelativePositionsBuffer(0),
	mParticleAttributeBuffers(0),
	mCLProgram_reorderAttributes(0)

{

	mObjectGenericFeaturesBuffer =
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneObjectGenericFeaturesBuffer"),
			//+1 because of the invalid object feature;
			(mNumMaxFluids + 1 + mNumMaxRigidBodies) * sizeof( CLshare::ObjectGenericFeatures ),
			true
		);

	//NOTE: even if mNumMaxRigidBodies == 0, we create a 128 byte buffer;
	//why? Because OpenCL kernels bug  if the don't get valid buffers passed to their
	//buffer arguments; NULL pointers don't seem to exist for kernel args ;(
	mRigidBodyBuffer =
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneRigidBodyBuffer"),
		    mNumMaxRigidBodies * sizeof( CLshare::ParticleRigidBody ),
			true
		);

	//TODO ensure that copies between a shared CL/GL Buffer and a pure CL buffer work bug free
	//note same stuff a above
	mRigidBodyRelativePositionsBuffer=
		SimulationResourceManager::getInstance().createGeneralPurposeOpenCLBuffer(
			String("particleSceneRigidBodyRelativePositionsBuffer"),
		    mNumMaxRigidBodies * mNumMaxParticlesPerRigidBody * sizeof( Vector4D ),
			true
		);

	mParticleAttributeBuffers = new ParticleAttributeBuffers(
		mNumTotalParticles, getInvalidObjectID(),
		//init to be save ;(
		true
	);


	//mCLProgram_reorderAttributes =

}

ParticleSceneRepresentation::~ParticleSceneRepresentation()
{
	delete mParticleAttributeBuffers;

	//TODO
	//delete mCLProgram_reorderAttributes;
}


//precondition: mParticleAttributeBuffers->mZIndicesPiPoBuffer has been sorted and
//				mParticleAttributeBuffers->mOldIndicesPiPoBuffer has been reordered accordingly
void ParticleSceneRepresentation::reorderAttributes()
{
	//TODO
	assert(0&&"TODO implement");

}

//upload mRigidBodyBuffer and mObjectGenericFeaturesBuffer to GPU bevor Simulation tick,
//as features may have been changed by the user;
void ParticleSceneRepresentation::flushObjectBuffers()
{
	//TODO

}

//to be called after a simulation step:
// mRigidBodyBuffer is read back and features are applied to managed ParticleRigidBody's;
void ParticleSceneRepresentation::synchronizeRigidBodies()
{
	//TODO
	assert(0&&"TODO implement");

}


//--------------------------------------------------------------------------------

//factory function; exception if not enough unassigned particles are left in the ParticleAttributeBuffers;
ParticleFluid* ParticleSceneRepresentation::createParticleFluid(
	String name,
	uint numParticles,
	//should be a ParticleLiquidVisualMaterial, but when simulating smoke, it would be a different one..
	//I'm tired of all those base class stubs; Maybe whis will be refactored once..
	VisualMaterial* visMat,
	ParticleFluidMechMat* mechMat) throw(BufferException)
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}

//factory function; exception if not enough unassigned particles are left in the ParticleAttributeBuffers;
ParticleRigidBody* ParticleSceneRepresentation::createParticleRigidBody(
	String name,
	uint numParticles,
	const AmendedTransform& initialGlobalTransform,
	//visual stuff for default rendering:
	const std::vector<SubObject*>& visualSubobjects,
	//should be a debug daw material or whatsoever
	VisualMaterial* particleVisualizationMat,
	Buffer* relativePositionsBuffer,
	ParticleFluidMechMat* mechMat) throw(BufferException)
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}


}

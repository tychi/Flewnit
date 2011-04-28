/*
 * ParticleSceneRepresentation
 *
 *  Created on: Apr 21, 2011
 *      Author: tychi
 */

#include "ParticleSceneRepresentation.h"

#include "ParticleAttributeBuffers.h"

#define FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE
#include "MPP/OpenCLProgram/ProgramSources/common/physicsDataStructures.cl"
#undef FLEWNIT_INCLUDED_BY_APPLICATION_SOURCE_CODE

#include "Simulator/SimulationResourceManager.h"
#include "Simulator/ParallelComputeManager.h"

#include "Buffer/Buffer.h"
#include "MPP/OpenCLProgram/CLProgram.h"
#include "Buffer/PingPongBuffer.h"
#include "MPP/OpenCLProgram/BasicCLProgram.h"
#include "Util/HelperFunctions.h"
#include "MPP/OpenCLProgram/CLKernelArguments.h"
#include "MPP/OpenCLProgram/ReorderParticleAttributesProgram.h"


namespace Flewnit
{


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
	mReorderParticleAttributesProgram(0)

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

	//mReorderParticleAttributesProgram = new ReorderParticleAttributesProgram(this);


}

ParticleSceneRepresentation::~ParticleSceneRepresentation()
{
	delete mParticleAttributeBuffers;


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


//bind the Buffers of ParticleMechanicsStage::mParticleAttributeBuffers to SimulationPipelineStage::mRenderingResults;
//called by ParticleSceneRepresentation's constructor;
void ParticleSceneRepresentation::associateParticleAttributeBuffersWithRenderingResults(
		std::map<BufferSemantics,BufferInterface*>& renderingResults)
{

	renderingResults[
	  //INDEX_SEMANTICS
	  mParticleAttributeBuffers->mParticleIndexTableBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mParticleIndexTableBuffer;

	renderingResults[
	  //PRIMITIVE_ID_SEMANTICS
	  mParticleAttributeBuffers->mObjectInfoPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mObjectInfoPiPoBuffer;

	renderingResults[
	  //Z_INDEX_SEMANTICS
	  mParticleAttributeBuffers->mZIndicesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mZIndicesPiPoBuffer;

	renderingResults[
	  //CUSTOM_SEMANTICS
	  mParticleAttributeBuffers->mOldIndicesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mOldIndicesPiPoBuffer;

	renderingResults[
	  //POSITION_SEMANTICS
	  mParticleAttributeBuffers->mPositionsPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mPositionsPiPoBuffer;

	renderingResults[
	  //DENSITY_SEMANTICS
	  mParticleAttributeBuffers->mDensitiesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mDensitiesPiPoBuffer;

	renderingResults[
	  //VELOCITY_SEMANTICS
	        //optional use mPredictedVelocitiesPiPoBuffer ;(
	  mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mCorrectedVelocitiesPiPoBuffer;

	renderingResults[
	  //FORCE_SEMANTICS
	  mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer->getBufferInfo().bufferSemantics
	] = mParticleAttributeBuffers->mLastStepsAccelerationsPiPoBuffer;

}





//internal helper routine for creating the geometry objects for fluid objects;
VertexBasedGeometry* ParticleSceneRepresentation::createGeometryFromAttributeBuffers(
		unsigned int particleStartIndex, unsigned int particleCount)
{
	//TODO
	assert(0&&"TODO implement");

	return 0;
}


}
